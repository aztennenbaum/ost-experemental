#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#define ASSERT(condition, ...) do { \
    if (!(condition)) { \
        fprintf(stderr, "ASSERTION FAILED at %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, "Condition: %s\n", #condition); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        exit(1); \
    } \
} while(0)

typedef struct {
    int m00, m10, m01;
} Component;

typedef struct {
    int x, y;
} Point;

typedef struct {
    int x0, x1, label;
} Run;

typedef struct {
    int *labels;
    Point *queue;
    Component *temp;
} RefContext;

typedef struct {
    Component *temp;
    int *parent;
    int *label_buffer;
} StdContext;

typedef struct {
    Component *temp;
    int *parent;
    int *label_buffer;
    int *active_count;
    int *free_after_row;
} OtfContext;

typedef struct {
    Component *temp;
    int *parent;
    int *label_live;
    int *free_after_row;
    int *touched_stamp;
    int *seen_stamp;
    Run *prev_runs;
    Run *curr_runs;
} OtfRunContext;

typedef struct {
    Component *temp;
    int *parent;
    int *label_buffer;
    int *active_count;
    int *free_after_row;
    int *prev_active_cols;
    int *curr_active_cols;
    Run *runs;
} OtfSparseContext;

static void insert_sorted(Component * restrict results, int *count, int max_results,
                         int m00, int m10, int m01)
{
    if (*count >= max_results && m00 <= results[max_results - 1].m00)
        return;

    int i = (*count < max_results) ? (*count)++ : max_results - 1;
    results[i] = (Component){m00, m10, m01};

    while (i > 0 && results[i].m00 > results[i - 1].m00) {
        Component tmp = results[i];
        results[i] = results[i - 1];
        results[i - 1] = tmp;
        i--;
    }
}

static int root_compress(int * restrict parent, int label)
{
    int root = label;
    while (parent[root] != root)
        root = parent[root];

    while (label != root) {
        int next = parent[label];
        parent[label] = root;
        label = next;
    }
    return root;
}

static void merge_components(Component * restrict keep, Component * restrict merge,
                            int * restrict parent, int keep_label, int merge_label)
{
    keep->m00 += merge->m00;
    keep->m10 += merge->m10;
    keep->m01 += merge->m01;
    parent[merge_label] = keep_label;
}

static int alloc_run_label(OtfRunContext *ctx, int max_labels, int row,
                           int *next_free_label)
{
    int start = *next_free_label;
    int label = start;

    do {
        if (!ctx->label_live[label] && ctx->free_after_row[label] < row) {
            ctx->label_live[label] = 1;
            ctx->parent[label] = label;
            ctx->temp[label] = (Component){0, 0, 0};

            label++;
            if (label >= max_labels) label = 1;
            *next_free_label = label;
            return label == 1 ? max_labels - 1 : label - 1;
        }

        label++;
        if (label >= max_labels) label = 1;
    } while (label != start);

    return 0;
}

static int merge_roots_run(OtfRunContext *ctx, int a, int b, int row)
{
    int ra = root_compress(ctx->parent, a);
    int rb = root_compress(ctx->parent, b);

    if (!ra) return rb;
    if (!rb) return ra;
    if (ra == rb) return ra;

    int keep = (ra < rb) ? ra : rb;
    int merge = (ra < rb) ? rb : ra;

    merge_components(&ctx->temp[keep], &ctx->temp[merge], ctx->parent, keep, merge);
    ctx->temp[merge] = (Component){0, 0, 0};
    ctx->label_live[merge] = 0;
    ctx->free_after_row[merge] = row + 1;

    return keep;
}

static int extract_runs_thresh(const unsigned char * restrict map_row, int w,
                               unsigned char threshold, Run *runs)
{
    int nr = 0;
    int x = 0;

    while (x < w) {
        while (x < w && map_row[x] <= threshold) x++;
        if (x >= w) break;

        runs[nr].x0 = x;
        while (x + 1 < w && map_row[x + 1] > threshold) x++;
        runs[nr].x1 = x;
        runs[nr].label = 0;
        nr++;
        x++;
    }

    return nr;
}

static int extract_runs_4(const unsigned char * restrict map_row, int w, Run *runs)
{
    return extract_runs_thresh(map_row, w, 0, runs);
}


static int alloc_reusable_label(Component * restrict temp,
                                int * restrict parent,
                                int * restrict active_count,
                                int * restrict free_after_row,
                                int max_labels, int row,
                                int *next_free_label)
{
    int start = *next_free_label;
    int label = start;

    do {
        if (active_count[label] == 0 && free_after_row[label] < row) {
            parent[label] = label;
            temp[label] = (Component){0, 0, 0};
            free_after_row[label] = row - 1;

            label++;
            if (label >= max_labels) label = 1;
            *next_free_label = label;
            return label == 1 ? max_labels - 1 : label - 1;
        }

        label++;
        if (label >= max_labels) label = 1;
    } while (label != start);

    return 0;
}

static int merge_active_roots(Component * restrict temp,
                              int * restrict parent,
                              int * restrict active_count,
                              int * restrict free_after_row,
                              int a, int b, int row)
{
    int ra = root_compress(parent, a);
    int rb = root_compress(parent, b);

    if (!ra) return rb;
    if (!rb) return ra;
    if (ra == rb) return ra;

    {
        int keep = (ra < rb) ? ra : rb;
        int merge = (ra < rb) ? rb : ra;

        merge_components(&temp[keep], &temp[merge], parent, keep, merge);
        active_count[keep] += active_count[merge];
        active_count[merge] = 0;
        temp[merge] = (Component){0, 0, 0};
        free_after_row[merge] = row + 1;
        return keep;
    }
}

static void close_active_column(Component * restrict temp,
                                int * restrict parent,
                                int * restrict label_buffer,
                                int * restrict active_count,
                                int * restrict free_after_row,
                                int x, int row,
                                const char *kind,
                                Component * restrict results,
                                int *count, int n)
{
    int top_label = label_buffer[x];
    if (!top_label)
        return;

    label_buffer[x] = 0;

    {
        int root = root_compress(parent, top_label);

        ASSERT(root > 0, "Invalid %s root %d at row=%d, x=%d", kind, root, row, x);
        ASSERT(active_count[root] > 0,
               "Non-positive %s active count for root %d at row=%d, x=%d",
               kind, root, row, x);

        if (--active_count[root] == 0) {
            insert_sorted(results, count, n,
                          temp[root].m00,
                          temp[root].m10,
                          temp[root].m01);
            temp[root] = (Component){0, 0, 0};
            parent[root] = root;
            free_after_row[root] = row - 1;
        }
    }
}

static int alloc_otf_label(OtfContext *ctx, int max_labels, int row,
                           int *next_free_label)
{
    return alloc_reusable_label(ctx->temp, ctx->parent, ctx->active_count,
                                ctx->free_after_row, max_labels, row,
                                next_free_label);
}

static int merge_roots_otf(OtfContext *ctx, int a, int b, int row)
{
    return merge_active_roots(ctx->temp, ctx->parent, ctx->active_count,
                              ctx->free_after_row, a, b, row);
}

static void close_otf_column(OtfContext *ctx, int x, int row,
                             Component * restrict results,
                             int *count, int n)
{
    close_active_column(ctx->temp, ctx->parent, ctx->label_buffer,
                        ctx->active_count, ctx->free_after_row,
                        x, row, "OTF", results, count, n);
}

static int alloc_sparse_label(OtfSparseContext *ctx, int max_labels, int row,
                              int *next_free_label)
{
    return alloc_reusable_label(ctx->temp, ctx->parent, ctx->active_count,
                                ctx->free_after_row, max_labels, row,
                                next_free_label);
}

static int merge_roots_sparse(OtfSparseContext *ctx, int a, int b, int row)
{
    return merge_active_roots(ctx->temp, ctx->parent, ctx->active_count,
                              ctx->free_after_row, a, b, row);
}

static void close_sparse_column(OtfSparseContext *ctx, int x, int row,
                                Component * restrict results,
                                int *count, int n)
{
    close_active_column(ctx->temp, ctx->parent, ctx->label_buffer,
                        ctx->active_count, ctx->free_after_row,
                        x, row, "sparse", results, count, n);
}

void print_map(const unsigned char *map, int w, int h)
{
    printf("Map pattern:\n");
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            printf("%c", map[y * w + x] ? '#' : '.');
        }
        printf("\n");
    }
    printf("\n");
}

int cc_ref(const unsigned char * restrict map, int w, int h, int n,
           Component * restrict results, RefContext *ctx)
{
    static const int dx[4] = {-1, 1, 0, 0}, dy[4] = {0, 0, -1, 1};
    int label = 1, count = 0;

    memset(ctx->labels, 0, w * h * sizeof(int));
    memset(ctx->temp, 0, (w * h / 2 + 1) * sizeof(Component));
    memset(results, 0, n * sizeof(Component));

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = y * w + x;
            if (!map[idx] || ctx->labels[idx]) continue;

            int m00 = 0, m10 = 0, m01 = 0, queue_head = 0, queue_tail = 0;
            ctx->labels[idx] = label;
            ctx->queue[queue_tail++] = (Point){x, y};

            while (queue_head < queue_tail) {
                Point p = ctx->queue[queue_head++];
                m00++;
                m10 += p.x;
                m01 += p.y;

                for (int i = 0; i < 4; i++) {
                    int nx = p.x + dx[i], ny = p.y + dy[i];

                    if ((unsigned)nx < (unsigned)w && (unsigned)ny < (unsigned)h) {
                        int ni = ny * w + nx;
                        if (map[ni] && !ctx->labels[ni]) {
                            ctx->labels[ni] = label;
                            ctx->queue[queue_tail++] = (Point){nx, ny};
                        }
                    }
                }
            }

            ctx->temp[label++] = (Component){m00, m10, m01};
        }
    }

    for (int i = 1; i < label; i++) {
        if (ctx->temp[i].m00) {
            insert_sorted(results, &count, n,
                         ctx->temp[i].m00, ctx->temp[i].m10, ctx->temp[i].m01);
        }
    }
    return count;
}

int cc_std(const unsigned char * restrict map, int w, int h, int n,
           Component * restrict results, StdContext *ctx)
{
    int label = 1, count = 0;
    const int max_labels = w * h / 2 + 1;

    memset(ctx->temp, 0, max_labels * sizeof(Component));
    memset(results, 0, n * sizeof(Component));
    memset(ctx->label_buffer, 0, w * sizeof(int));

    for (int i = 0; i < max_labels; i++)
        ctx->parent[i] = i;

    for (int y = 0; y < h; y++) {
        const unsigned char *map_row = &map[y * w];

        for (int x = 0; x < w; x++) {
            if (!map_row[x]) {
                ctx->label_buffer[x] = 0;
                continue;
            }

            int left = (x > 0) ? root_compress(ctx->parent, ctx->label_buffer[x - 1]) : 0;
            int top = root_compress(ctx->parent, ctx->label_buffer[x]);
            int assigned;

            if (!left && !top) {
                assigned = label++;
            } else if (left && top && left != top) {
                int keep = (left < top) ? left : top;
                int merge = (left < top) ? top : left;
                merge_components(&ctx->temp[keep], &ctx->temp[merge],
                               ctx->parent, keep, merge);
                assigned = keep;
            } else {
                assigned = left ? left : top;
            }

            ctx->label_buffer[x] = assigned;
            ctx->temp[assigned].m00++;
            ctx->temp[assigned].m10 += x;
            ctx->temp[assigned].m01 += y;
        }
    }

    for (int i = 1; i < label; i++) {
        if (ctx->parent[i] == i && ctx->temp[i].m00) {
            insert_sorted(results, &count, n,
                         ctx->temp[i].m00, ctx->temp[i].m10, ctx->temp[i].m01);
        }
    }
    return count;
}

int cc_otf(const unsigned char * restrict map, int w, int h, int n,
           Component * restrict results, OtfContext *ctx)
{
    const int max_labels = w + 1;
    int next_free_label = 1;
    int count = 0;

    memset(results, 0, n * sizeof(Component));
    memset(ctx->temp, 0, max_labels * sizeof(Component));
    memset(ctx->label_buffer, 0, w * sizeof(int));
    memset(ctx->active_count, 0, max_labels * sizeof(int));
    memset(ctx->free_after_row, -1, max_labels * sizeof(int));

    for (int i = 0; i < max_labels; i++)
        ctx->parent[i] = i;

    for (int y = 0; y < h; y++) {
        const unsigned char * restrict map_row = &map[y * w];

        for (int x = 0; x < w; x++) {
            int top_label = ctx->label_buffer[x];

            if (!map_row[x]) {
                close_otf_column(ctx, x, y, results, &count, n);
                continue;
            }

            {
                int left = (x > 0 && ctx->label_buffer[x - 1] > 0)
                    ? root_compress(ctx->parent, ctx->label_buffer[x - 1]) : 0;
                int top = (top_label > 0)
                    ? root_compress(ctx->parent, top_label) : 0;
                int assigned;

                if (left && top && left != top) {
                    assigned = merge_roots_otf(ctx, left, top, y);
                } else if (left || top) {
                    assigned = left ? left : top;
                } else {
                    assigned = alloc_otf_label(ctx, max_labels, y, &next_free_label);
                    if (!assigned) {
                        fprintf(stderr, "cc_otf: out of labels at row %d\n", y);
                        return -1;
                    }
                }

                assigned = root_compress(ctx->parent, assigned);

                if (!top_label)
                    ctx->active_count[assigned]++;

                ctx->label_buffer[x] = assigned;
                ctx->temp[assigned].m00++;
                ctx->temp[assigned].m10 += x;
                ctx->temp[assigned].m01 += y;
            }
        }
    }

    for (int x = 0; x < w; x++)
        close_otf_column(ctx, x, h, results, &count, n);

    return count;
}

int cc_otf_sparse_thresh(const unsigned char * restrict map, int w, int h, int n,
                        unsigned char threshold,
                        Component * restrict results, OtfSparseContext *ctx)
{
    const int max_labels = w + 1;
    int next_free_label = 1;
    int count = 0;
    int nr_prev_active = 0;

    memset(results, 0, n * sizeof(Component));
    memset(ctx->temp, 0, max_labels * sizeof(Component));
    memset(ctx->label_buffer, 0, w * sizeof(int));
    memset(ctx->active_count, 0, max_labels * sizeof(int));
    memset(ctx->free_after_row, -1, max_labels * sizeof(int));

    for (int i = 0; i < max_labels; i++)
        ctx->parent[i] = i;

    for (int y = 0; y < h; y++) {
        const unsigned char * restrict map_row = &map[y * w];
        int nr_runs = extract_runs_thresh(map_row, w, threshold, ctx->runs);
        int nr_curr_active = 0;
        int p = 0;

        for (int j = 0; j < nr_runs; j++) {
            int x0 = ctx->runs[j].x0;
            int x1 = ctx->runs[j].x1;
            int assigned = 0;
            int q;

            while (p < nr_prev_active && ctx->prev_active_cols[p] < x0) {
                close_sparse_column(ctx, ctx->prev_active_cols[p], y,
                                    results, &count, n);
                p++;
            }

            for (q = p; q < nr_prev_active && ctx->prev_active_cols[q] <= x1; q++) {
                int x = ctx->prev_active_cols[q];
                int root = root_compress(ctx->parent, ctx->label_buffer[x]);

                if (!assigned) assigned = root;
                else assigned = merge_roots_sparse(ctx, assigned, root, y);
            }

            if (!assigned) {
                assigned = alloc_sparse_label(ctx, max_labels, y, &next_free_label);
                if (!assigned) {
                    fprintf(stderr, "cc_otf_sparse_thresh: out of labels at row %d\n", y);
                    return -1;
                }
            }

            assigned = root_compress(ctx->parent, assigned);

            for (int x = x0; x <= x1; x++) {
                if (!ctx->label_buffer[x])
                    ctx->active_count[assigned]++;

                ctx->label_buffer[x] = assigned;
                ctx->curr_active_cols[nr_curr_active++] = x;
                ctx->temp[assigned].m00++;
                ctx->temp[assigned].m10 += x;
                ctx->temp[assigned].m01 += y;
            }

            p = q;
        }

        while (p < nr_prev_active) {
            close_sparse_column(ctx, ctx->prev_active_cols[p], y,
                                results, &count, n);
            p++;
        }

        {
            int *tmp = ctx->prev_active_cols;
            ctx->prev_active_cols = ctx->curr_active_cols;
            ctx->curr_active_cols = tmp;
        }
        nr_prev_active = nr_curr_active;
    }

    for (int i = 0; i < nr_prev_active; i++) {
        close_sparse_column(ctx, ctx->prev_active_cols[i], h,
                            results, &count, n);
    }

    return count;
}

int cc_otf_sparse(const unsigned char * restrict map, int w, int h, int n,
                  Component * restrict results, OtfSparseContext *ctx)
{
    return cc_otf_sparse_thresh(map, w, h, n, 0, results, ctx);
}

int cc_otf_run(const unsigned char * restrict map, int w, int h, int n,
               Component * restrict results, OtfRunContext *ctx)
{
    const int max_labels = w + 1;
    int next_free_label = 1;
    int count = 0;
    int nr_prev = 0;

    memset(results, 0, n * sizeof(Component));
    memset(ctx->temp, 0, max_labels * sizeof(Component));
    memset(ctx->label_live, 0, max_labels * sizeof(int));
    memset(ctx->free_after_row, -1, max_labels * sizeof(int));
    memset(ctx->touched_stamp, 0, max_labels * sizeof(int));
    memset(ctx->seen_stamp, 0, max_labels * sizeof(int));

    for (int i = 0; i < max_labels; i++) {
        ctx->parent[i] = i;
    }

    for (int y = 0; y < h; y++) {
        const unsigned char * restrict map_row = &map[y * w];
        int stamp = y + 1;
        int nr_curr = extract_runs_4(map_row, w, ctx->curr_runs);
        int p = 0;

        for (int j = 0; j < nr_curr; j++) {
            Run *cr = &ctx->curr_runs[j];
            int assigned = 0;
            int len = cr->x1 - cr->x0 + 1;

            while (p < nr_prev && ctx->prev_runs[p].x1 < cr->x0) {
                p++;
            }

            for (int q = p; q < nr_prev && ctx->prev_runs[q].x0 <= cr->x1; q++) {
                int root = root_compress(ctx->parent, ctx->prev_runs[q].label);
                ctx->touched_stamp[root] = stamp;

                if (!assigned) assigned = root;
                else assigned = merge_roots_run(ctx, assigned, root, y);
            }

            if (!assigned) {
                assigned = alloc_run_label(ctx, max_labels, y, &next_free_label);
                if (!assigned) {
                    fprintf(stderr, "cc_otf_run: out of labels at row %d\n", y);
                    return -1;
                }
            }

            assigned = root_compress(ctx->parent, assigned);
            cr->label = assigned;
            ctx->label_live[assigned] = 1;
            ctx->temp[assigned].m00 += len;
            ctx->temp[assigned].m10 += (cr->x0 + cr->x1) * len / 2;
            ctx->temp[assigned].m01 += y * len;
        }

        for (int i = 0; i < nr_prev; i++) {
            int root = root_compress(ctx->parent, ctx->prev_runs[i].label);
            if (ctx->seen_stamp[root] == stamp) continue;
            ctx->seen_stamp[root] = stamp;

            if (ctx->touched_stamp[root] != stamp) {
                insert_sorted(results, &count, n,
                              ctx->temp[root].m00,
                              ctx->temp[root].m10,
                              ctx->temp[root].m01);
                ctx->temp[root] = (Component){0, 0, 0};
                ctx->label_live[root] = 0;
                ctx->free_after_row[root] = y;
                ctx->parent[root] = root;
            }
        }

        {
            Run *tmp = ctx->prev_runs;
            ctx->prev_runs = ctx->curr_runs;
            ctx->curr_runs = tmp;
        }
        nr_prev = nr_curr;
    }

    {
        int stamp = h + 1;
        for (int i = 0; i < nr_prev; i++) {
            int root = root_compress(ctx->parent, ctx->prev_runs[i].label);
            if (ctx->seen_stamp[root] == stamp) continue;
            ctx->seen_stamp[root] = stamp;

            insert_sorted(results, &count, n,
                          ctx->temp[root].m00,
                          ctx->temp[root].m10,
                          ctx->temp[root].m01);
            ctx->temp[root] = (Component){0, 0, 0};
            ctx->label_live[root] = 0;
            ctx->free_after_row[root] = h;
            ctx->parent[root] = root;
        }
    }

    return count;
}

static int g_noise_permille = 500;

void test_noise_permille(unsigned char *map, int w, int h, int permille)
{
    if (permille < 0) permille = 0;
    if (permille > 1000) permille = 1000;

    memset(map, 0, w * h);
    srand(42);
    for (int i = 0; i < w * h; i++) {
        map[i] = (rand() % 1000) < permille;
    }
}

void test_noise(unsigned char *map, int w, int h)
{
    test_noise_permille(map, w, h, g_noise_permille);
}

void test_grid(unsigned char *map, int w, int h)
{
    memset(map, 0, w * h);
    for (int y = 0; y < h; y += 30) {
        for (int x = 0; x < w; x += 30) {
            for (int dy = 0; dy < 10 && y + dy < h; dy++) {
                for (int dx = 0; dx < 10 && x + dx < w; dx++) {
                    map[(y + dy) * w + x + dx] = 1;
                }
            }
        }
    }
}

void test_large(unsigned char *map, int w, int h)
{
    memset(map, 1, w * h);
}

void test_check(unsigned char *map, int w, int h)
{
    memset(map, 0, w * h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            map[y * w + x] = ((x / 2 + y / 2) & 1);
        }
    }
}

void test_snake(unsigned char *map, int w, int h)
{
    memset(map, 0, w * h);
    for (int y = 0; y < h; y++) {
        if (y & 1) {
            for (int x = w - 1; x >= 0; x--) map[y * w + x] = 1;
        } else {
            for (int x = 0; x < w; x++) map[y * w + x] = 1;
        }
    }
}

int test_and_compare(const char *name, const unsigned char *map, int w, int h, int n)
{
    const int max_labels_std = w * h / 2 + 1;
    const int max_labels_otf = w + 1;
    const int max_runs = (w + 1) / 2;

    Component *results_ref = calloc(n, sizeof(Component));
    Component *results_std = calloc(n, sizeof(Component));
    Component *results_otf = calloc(n, sizeof(Component));
    Component *results_run = calloc(n, sizeof(Component));
    Component *results_sparse = calloc(n, sizeof(Component));

    RefContext ctx_ref = {
        .labels = malloc(w * h * sizeof(int)),
        .queue = malloc(w * h * sizeof(Point)),
        .temp = malloc(max_labels_std * sizeof(Component))
    };

    StdContext ctx_std = {
        .temp = malloc(max_labels_std * sizeof(Component)),
        .parent = malloc(max_labels_std * sizeof(int)),
        .label_buffer = malloc(w * sizeof(int))
    };

    OtfContext ctx_otf = {
        .temp = malloc(max_labels_otf * sizeof(Component)),
        .parent = malloc(max_labels_otf * sizeof(int)),
        .label_buffer = malloc(w * sizeof(int)),
        .active_count = malloc(max_labels_otf * sizeof(int)),
        .free_after_row = malloc(max_labels_otf * sizeof(int))
    };

    OtfRunContext ctx_run = {
        .temp = malloc(max_labels_otf * sizeof(Component)),
        .parent = malloc(max_labels_otf * sizeof(int)),
        .label_live = malloc(max_labels_otf * sizeof(int)),
        .free_after_row = malloc(max_labels_otf * sizeof(int)),
        .touched_stamp = malloc(max_labels_otf * sizeof(int)),
        .seen_stamp = malloc(max_labels_otf * sizeof(int)),
        .prev_runs = malloc(max_runs * sizeof(Run)),
        .curr_runs = malloc(max_runs * sizeof(Run))
    };

    OtfSparseContext ctx_sparse = {
        .temp = malloc(max_labels_otf * sizeof(Component)),
        .parent = malloc(max_labels_otf * sizeof(int)),
        .label_buffer = malloc(w * sizeof(int)),
        .active_count = malloc(max_labels_otf * sizeof(int)),
        .free_after_row = malloc(max_labels_otf * sizeof(int)),
        .prev_active_cols = malloc(w * sizeof(int)),
        .curr_active_cols = malloc(w * sizeof(int)),
        .runs = malloc(max_runs * sizeof(Run))
    };

    cc_ref(map, w, h, n, results_ref, &ctx_ref);
    cc_std(map, w, h, n, results_std, &ctx_std);
    cc_otf(map, w, h, n, results_otf, &ctx_otf);
    cc_otf_run(map, w, h, n, results_run, &ctx_run);
    cc_otf_sparse(map, w, h, n, results_sparse, &ctx_sparse);

    int ok_std = 1, ok_otf = 1, ok_run = 1, ok_sparse = 1;
    for (int i = 0; i < n; i++) {
        if (results_std[i].m00 != results_ref[i].m00) ok_std = 0;
        if (results_otf[i].m00 != results_ref[i].m00) ok_otf = 0;
        if (results_run[i].m00 != results_ref[i].m00) ok_run = 0;
        if (results_sparse[i].m00 != results_ref[i].m00) ok_sparse = 0;
    }

    if (name && (!ok_std || !ok_otf || !ok_run || !ok_sparse)) {
        printf("\n=== FAILURE: %s (%dx%d) ===\n", name, w, h);
        
        if (w <= 80 && h <= 80) {
            print_map(map, w, h);
        }
        
        printf("Rank  Ref(area)  Std(area)  OTF(area)  Run(area)\n");
        printf("------------------------------------------------\n");
        for (int i = 0; i < n; i++) {
            int diff_std = (results_std[i].m00 != results_ref[i].m00);
            int diff_otf = (results_otf[i].m00 != results_ref[i].m00);
            int diff_run = (results_run[i].m00 != results_ref[i].m00);
            if (!diff_std && !diff_otf && !diff_run) continue;
            printf("[%2d]  %7d    %7d    %7d    %7d\n",
                   i, results_ref[i].m00, results_std[i].m00,
                   results_otf[i].m00, results_run[i].m00);
        }
    }

    free(results_ref);
    free(results_std);
    free(results_otf);
    free(results_run);
    free(results_sparse);
    free(ctx_ref.labels);
    free(ctx_ref.queue);
    free(ctx_ref.temp);
    free(ctx_std.temp);
    free(ctx_std.parent);
    free(ctx_std.label_buffer);
    free(ctx_otf.temp);
    free(ctx_otf.parent);
    free(ctx_otf.label_buffer);
    free(ctx_otf.active_count);
    free(ctx_otf.free_after_row);
    free(ctx_run.temp);
    free(ctx_run.parent);
    free(ctx_run.label_live);
    free(ctx_run.free_after_row);
    free(ctx_run.touched_stamp);
    free(ctx_run.seen_stamp);
    free(ctx_run.prev_runs);
    free(ctx_run.curr_runs);
    free(ctx_sparse.temp);
    free(ctx_sparse.parent);
    free(ctx_sparse.label_buffer);
    free(ctx_sparse.active_count);
    free(ctx_sparse.free_after_row);
    free(ctx_sparse.prev_active_cols);
    free(ctx_sparse.curr_active_cols);
    free(ctx_sparse.runs);

    return (ok_std << 3) | (ok_otf << 2) | (ok_run << 1) | ok_sparse;
}

void binary_search_failure(void (*test_func)(unsigned char *, int, int),
                          const char *test_name, int max_size, int n)
{
    printf("\n  Binary search for minimal failure...\n");

    int low = 4, high = max_size;
    int min_fail = max_size;

    while (low <= high) {
        int mid = (low + high) / 2;
        unsigned char *test_map = malloc(mid * mid);
        test_func(test_map, mid, mid);

        int result = test_and_compare(NULL, test_map, mid, mid, n);
        int pass = ((result & 7) == 7);

        printf("    %dx%d: %s\n", mid, mid, pass ? "PASS" : "FAIL");

        if (!pass) {
            min_fail = mid;
            high = mid - 1;
        } else {
            low = mid + 1;
        }
        free(test_map);
    }

    printf("  Minimal failing size: %dx%d\n\n", min_fail, min_fail);
    printf("=== Printing pattern and rerunning with DEBUG enabled ===\n");

    {
        unsigned char *fail_map = malloc(min_fail * min_fail);
        test_func(fail_map, min_fail, min_fail);
        test_and_compare(test_name, fail_map, min_fail, min_fail, n);
        free(fail_map);
    }
}

void benchmark(const char *name, void (*test_func)(unsigned char *, int, int),
               int w, int h, int n, int iterations)
{
    const int max_labels_std = w * h / 2 + 1;
    const int max_labels_otf = w + 1;
    const int max_runs = (w + 1) / 2;

    unsigned char *map = malloc(w * h);
    Component *results_ref = calloc(n, sizeof(Component));
    Component *results_std = calloc(n, sizeof(Component));
    Component *results_otf = calloc(n, sizeof(Component));
    Component *results_run = calloc(n, sizeof(Component));
    Component *results_sparse = calloc(n, sizeof(Component));

    RefContext ctx_ref = {
        .labels = malloc(w * h * sizeof(int)),
        .queue = malloc(w * h * sizeof(Point)),
        .temp = malloc(max_labels_std * sizeof(Component))
    };

    StdContext ctx_std = {
        .temp = malloc(max_labels_std * sizeof(Component)),
        .parent = malloc(max_labels_std * sizeof(int)),
        .label_buffer = malloc(w * sizeof(int))
    };

    OtfContext ctx_otf = {
        .temp = malloc(max_labels_otf * sizeof(Component)),
        .parent = malloc(max_labels_otf * sizeof(int)),
        .label_buffer = malloc(w * sizeof(int)),
        .active_count = malloc(max_labels_otf * sizeof(int)),
        .free_after_row = malloc(max_labels_otf * sizeof(int))
    };

    OtfRunContext ctx_run = {
        .temp = malloc(max_labels_otf * sizeof(Component)),
        .parent = malloc(max_labels_otf * sizeof(int)),
        .label_live = malloc(max_labels_otf * sizeof(int)),
        .free_after_row = malloc(max_labels_otf * sizeof(int)),
        .touched_stamp = malloc(max_labels_otf * sizeof(int)),
        .seen_stamp = malloc(max_labels_otf * sizeof(int)),
        .prev_runs = malloc(max_runs * sizeof(Run)),
        .curr_runs = malloc(max_runs * sizeof(Run))
    };

    OtfSparseContext ctx_sparse = {
        .temp = malloc(max_labels_otf * sizeof(Component)),
        .parent = malloc(max_labels_otf * sizeof(int)),
        .label_buffer = malloc(w * sizeof(int)),
        .active_count = malloc(max_labels_otf * sizeof(int)),
        .free_after_row = malloc(max_labels_otf * sizeof(int)),
        .prev_active_cols = malloc(w * sizeof(int)),
        .curr_active_cols = malloc(w * sizeof(int)),
        .runs = malloc(max_runs * sizeof(Run))
    };

    double time_ref = 0, time_std = 0, time_otf = 0, time_run = 0, time_sparse = 0;
    test_func(map, w, h);

    for (int i = 0; i < iterations; i++) {
        clock_t t = clock();
        cc_ref(map, w, h, n, results_ref, &ctx_ref);
        time_ref += (double)(clock() - t) / CLOCKS_PER_SEC;
    }

    for (int i = 0; i < iterations; i++) {
        clock_t t = clock();
        cc_std(map, w, h, n, results_std, &ctx_std);
        time_std += (double)(clock() - t) / CLOCKS_PER_SEC;
    }

    for (int i = 0; i < iterations; i++) {
        clock_t t = clock();
        cc_otf(map, w, h, n, results_otf, &ctx_otf);
        time_otf += (double)(clock() - t) / CLOCKS_PER_SEC;
    }

    for (int i = 0; i < iterations; i++) {
        clock_t t = clock();
        cc_otf_run(map, w, h, n, results_run, &ctx_run);
        time_run += (double)(clock() - t) / CLOCKS_PER_SEC;
    }

    for (int i = 0; i < iterations; i++) {
        clock_t t = clock();
        cc_otf_sparse(map, w, h, n, results_sparse, &ctx_sparse);
        time_sparse += (double)(clock() - t) / CLOCKS_PER_SEC;
    }

    int ok_std = 1, ok_otf = 1, ok_run = 1, ok_sparse = 1;
    for (int i = 0; i < n; i++) {
        if (results_std[i].m00 != results_ref[i].m00) ok_std = 0;
        if (results_otf[i].m00 != results_ref[i].m00) ok_otf = 0;
        if (results_run[i].m00 != results_ref[i].m00) ok_run = 0;
        if (results_sparse[i].m00 != results_ref[i].m00) ok_sparse = 0;
    }

    {
        int mem_otf = (max_labels_otf * sizeof(Component) +
                       w * sizeof(int) +
                       max_labels_otf * sizeof(int) +
                       max_labels_otf * sizeof(int)) / 1024;
        int mem_run = (max_labels_otf * sizeof(Component) +
                       5 * max_labels_otf * sizeof(int) +
                       2 * max_runs * sizeof(Run)) / 1024;
        int mem_sparse = (max_labels_otf * sizeof(Component) +
                          4 * max_labels_otf * sizeof(int) +
                          3 * w * sizeof(int) +
                          max_runs * sizeof(Run)) / 1024;

        printf("%-18s %4dx%-4d %c%c%c%c %7.4f %7.4f %7.4f %7.4f %7.4f %5.1fx %5.1fx %5.1fx %5.1fx %5dK %5dK %5dK\n",
               name, w, h,
               ok_std ? 'Y' : 'N', ok_otf ? 'Y' : 'N', ok_run ? 'Y' : 'N', ok_sparse ? 'Y' : 'N',
               time_ref / iterations, time_std / iterations,
               time_otf / iterations, time_run / iterations, time_sparse / iterations,
               time_std > 0 ? time_ref / time_std : 0,
               time_otf > 0 ? time_ref / time_otf : 0,
               time_run > 0 ? time_ref / time_run : 0,
               time_sparse > 0 ? time_ref / time_sparse : 0,
               mem_otf, mem_run, mem_sparse);
    }

    if (!ok_otf || !ok_run || !ok_sparse) {
        binary_search_failure(test_func, name, w < h ? w : h, n);
    }

    free(map);
    free(results_ref);
    free(results_std);
    free(results_otf);
    free(results_run);
    free(results_sparse);
    free(ctx_ref.labels);
    free(ctx_ref.queue);
    free(ctx_ref.temp);
    free(ctx_std.temp);
    free(ctx_std.parent);
    free(ctx_std.label_buffer);
    free(ctx_otf.temp);
    free(ctx_otf.parent);
    free(ctx_otf.label_buffer);
    free(ctx_otf.active_count);
    free(ctx_otf.free_after_row);
    free(ctx_run.temp);
    free(ctx_run.parent);
    free(ctx_run.label_live);
    free(ctx_run.free_after_row);
    free(ctx_run.touched_stamp);
    free(ctx_run.seen_stamp);
    free(ctx_run.prev_runs);
    free(ctx_run.curr_runs);
    free(ctx_sparse.temp);
    free(ctx_sparse.parent);
    free(ctx_sparse.label_buffer);
    free(ctx_sparse.active_count);
    free(ctx_sparse.free_after_row);
    free(ctx_sparse.prev_active_cols);
    free(ctx_sparse.curr_active_cols);
    free(ctx_sparse.runs);
}

void benchmark_noise_sweep(int w, int h, int n, int iterations)
{
    static const int permille[] = {
        0, 1, 2, 5,
        10, 20, 50,
        100, 200, 300, 400, 500, 600, 700, 800, 900, 1000
    };
    int nr = (int)(sizeof(permille) / sizeof(permille[0]));

    for (int i = 0; i < nr; i++) {
        char name[64];
        g_noise_permille = permille[i];
        snprintf(name, sizeof(name), "2K Noise %d.%d%%",
                 g_noise_permille / 10, g_noise_permille % 10);
        benchmark(name, test_noise, w, h, n, iterations);
    }
}

int main(void)
{
    printf("Connected Components - Cleaned OTF/Sparse Variants\n\n");
    printf("%-18s %9s %s %7s %7s %7s %7s %7s %6s %6s %6s %6s %6s %6s %6s\n",
           "Test", "Size", "Ok", "Ref(s)", "Std(s)", "OTF(s)", "Run(s)", "Spr(s)",
           "Spd-S", "Spd-O", "Spd-R", "Spd-P", "Mem-O", "Mem-R", "Mem-P");
    printf("----------------------------------------------------------------------------------------------------------------\n");

    printf("=== 2K Noise Sweep ===\n");
    benchmark_noise_sweep(2048, 2048, 100, 1);

    printf("\n=== 2K Structured Test Cases ===\n");
    benchmark("2K Grid", test_grid, 2048, 2048, 100, 1);
    benchmark("2K Large", test_large, 2048, 2048, 100, 1);
    benchmark("2K Checkerboard", test_check, 2048, 2048, 100, 1);
    benchmark("2K Snake", test_snake, 2048, 2048, 100, 1);

    return 0;
}


