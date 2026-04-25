#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int m00, m10, m01;
} Component;

typedef struct {
    int x, y;
} Point;

typedef struct {
    int *visited;
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
    int *col_label;
    int *active_count;
    int *free_after_row;
} OtfContext;

static inline void insert_sorted(Component *restrict results, int *count, int max_results,
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

static inline int find_root(int *restrict parent, int label)
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

static inline void merge_components(Component *restrict keep, Component *restrict merge,
                                    int *restrict parent, int keep_label, int merge_label)
{
    keep->m00 += merge->m00;
    keep->m10 += merge->m10;
    keep->m01 += merge->m01;
    parent[merge_label] = keep_label;
}

static inline int alloc_label(Component *restrict temp,
                              int *restrict parent,
                              int *restrict active_count,
                              int *restrict free_after_row,
                              int max_labels, int row,
                              int *restrict next_free_label)
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

static inline int merge_labels(Component *restrict temp,
                               int *restrict parent,
                               int *restrict active_count,
                               int *restrict free_after_row,
                               int a, int b, int row)
{
    int ra = find_root(parent, a);
    int rb = find_root(parent, b);

    if (!ra) return rb;
    if (!rb) return ra;
    if (ra == rb) return ra;

    int keep = (ra < rb) ? ra : rb;
    int merge = (ra < rb) ? rb : ra;

    temp[keep].m00 += temp[merge].m00;
    temp[keep].m10 += temp[merge].m10;
    temp[keep].m01 += temp[merge].m01;
    parent[merge] = keep;
    active_count[keep] += active_count[merge];
    active_count[merge] = 0;
    temp[merge] = (Component){0, 0, 0};
    free_after_row[merge] = row + 1;

    return keep;
}

static inline void close_column(Component *restrict temp,
                                int *restrict parent,
                                int *restrict col_label,
                                int *restrict active_count,
                                int *restrict free_after_row,
                                int x, int row,
                                Component *restrict results,
                                int *count, int n)
{
    int label = col_label[x];
    if (!label)
        return;

    col_label[x] = 0;
    int root = find_root(parent, label);

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

int cc_ref(const unsigned char *restrict map, int w, int h, int n,
           Component *restrict results, RefContext *ctx)
{
    static const int dx[4] = {-1, 1, 0, 0}, dy[4] = {0, 0, -1, 1};
    int label = 1, count = 0;

    memset(ctx->visited, 0, w * h * sizeof(int));
    memset(ctx->temp, 0, (w * h / 2 + 1) * sizeof(Component));
    memset(results, 0, n * sizeof(Component));

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = y * w + x;
            if (!map[idx] || ctx->visited[idx]) continue;

            int m00 = 0, m10 = 0, m01 = 0, queue_head = 0, queue_tail = 0;
            ctx->visited[idx] = label;
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
                        if (map[ni] && !ctx->visited[ni]) {
                            ctx->visited[ni] = label;
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

int cc_std(const unsigned char *restrict map, int w, int h, int n,
           Component *restrict results, StdContext *ctx)
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

            int left = (x > 0) ? find_root(ctx->parent, ctx->label_buffer[x - 1]) : 0;
            int top = find_root(ctx->parent, ctx->label_buffer[x]);
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

int cc_otf(const unsigned char *restrict map, int w, int h, int n,
           Component *restrict results, OtfContext *ctx)
{
    const int max_labels = w + 1;
    int next_free_label = 1;
    int count = 0;

    memset(results, 0, n * sizeof(Component));
    memset(ctx->temp, 0, max_labels * sizeof(Component));
    memset(ctx->col_label, 0, w * sizeof(int));
    memset(ctx->active_count, 0, max_labels * sizeof(int));
    memset(ctx->free_after_row, -1, max_labels * sizeof(int));

    for (int i = 0; i < max_labels; i++)
        ctx->parent[i] = i;

    for (int y = 0; y < h; y++) {
        const unsigned char *restrict map_row = &map[y * w];

        for (int x = 0; x < w; x++) {
            int top_label = ctx->col_label[x];

            if (!map_row[x]) {
                close_column(ctx->temp, ctx->parent, ctx->col_label,
                            ctx->active_count, ctx->free_after_row,
                            x, y, results, &count, n);
                continue;
            }

            int left = (x > 0 && ctx->col_label[x - 1])
                ? find_root(ctx->parent, ctx->col_label[x - 1]) : 0;
            int top = top_label ? find_root(ctx->parent, top_label) : 0;
            int assigned;

            if (left && top && left != top) {
                assigned = merge_labels(ctx->temp, ctx->parent,
                                       ctx->active_count, ctx->free_after_row,
                                       left, top, y);
            } else if (left || top) {
                assigned = left | top;  // bitwise OR works since only one is non-zero
            } else {
                assigned = alloc_label(ctx->temp, ctx->parent,
                                      ctx->active_count, ctx->free_after_row,
                                      max_labels, y, &next_free_label);
                if (!assigned) {
                    fprintf(stderr, "cc_otf: out of labels at row %d\n", y);
                    return -1;
                }
            }

            if (!top_label)
                ctx->active_count[assigned]++;

            ctx->col_label[x] = assigned;
            ctx->temp[assigned].m00++;
            ctx->temp[assigned].m10 += x;
            ctx->temp[assigned].m01 += y;
        }
    }

    for (int x = 0; x < w; x++)
        close_column(ctx->temp, ctx->parent, ctx->col_label,
                    ctx->active_count, ctx->free_after_row,
                    x, h, results, &count, n);

    return count;
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

void benchmark(const char *name, void (*test_func)(unsigned char *, int, int),
               int w, int h, int n, int iterations)
{
    const int max_labels_std = w * h / 2 + 1;
    const int max_labels_otf = w + 1;

    unsigned char *map = malloc(w * h);
    Component *results_ref = calloc(n, sizeof(Component));
    Component *results_std = calloc(n, sizeof(Component));
    Component *results_otf = calloc(n, sizeof(Component));

    RefContext ctx_ref = {
        .visited = malloc(w * h * sizeof(int)),
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
        .col_label = malloc(w * sizeof(int)),
        .active_count = malloc(max_labels_otf * sizeof(int)),
        .free_after_row = malloc(max_labels_otf * sizeof(int))
    };

    double time_ref = 0, time_std = 0, time_otf = 0;
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

    int ok_std = 1, ok_otf = 1;
    for (int i = 0; i < n; i++) {
        if (results_std[i].m00 != results_ref[i].m00) ok_std = 0;
        if (results_otf[i].m00 != results_ref[i].m00) ok_otf = 0;
    }

    int mem_otf = (max_labels_otf * sizeof(Component) +
                   w * sizeof(int) +
                   3 * max_labels_otf * sizeof(int)) / 1024;

    printf("%-18s %4dx%-4d %c%c %7.4f %7.4f %7.4f %5.1fx %5.1fx %5dK\n",
           name, w, h,
           ok_std ? 'Y' : 'N', ok_otf ? 'Y' : 'N',
           time_ref / iterations,
           time_std / iterations,
           time_otf / iterations,
           time_std > 0 ? time_ref / time_std : 0,
           time_otf > 0 ? time_ref / time_otf : 0,
           mem_otf);

    if ((!ok_std || !ok_otf) && w <= 80 && h <= 80) {
        printf("\n=== FAILURE: %s (%dx%d) ===\n", name, w, h);
        print_map(map, w, h);
        printf("Rank  Ref(area)  Std(area)  OTF(area)\n");
        printf("----------------------------------------\n");
        for (int i = 0; i < n; i++) {
            int diff_std = (results_std[i].m00 != results_ref[i].m00);
            int diff_otf = (results_otf[i].m00 != results_ref[i].m00);
            if (diff_std || diff_otf) {
                printf("[%2d]  %7d    %7d    %7d\n",
                       i, results_ref[i].m00, results_std[i].m00,
                       results_otf[i].m00);
            }
        }
    }

    free(map);
    free(results_ref);
    free(results_std);
    free(results_otf);
    free(ctx_ref.visited);
    free(ctx_ref.queue);
    free(ctx_ref.temp);
    free(ctx_std.temp);
    free(ctx_std.parent);
    free(ctx_std.label_buffer);
    free(ctx_otf.temp);
    free(ctx_otf.parent);
    free(ctx_otf.col_label);
    free(ctx_otf.active_count);
    free(ctx_otf.free_after_row);
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
    printf("Connected Components - Std vs OTF\n\n");
    printf("%-18s %9s %s %7s %7s %7s %6s %6s %6s\n",
           "Test", "Size", "Ok", "Ref(s)", "Std(s)", "OTF(s)", "Spd-S", "Spd-O", "Mem-O");
    printf("------------------------------------------------------------------------------------\n");

    printf("=== 2K Noise Sweep ===\n");
    benchmark_noise_sweep(2048, 2048, 100, 1);

    printf("\n=== 2K Structured Test Cases ===\n");
    benchmark("2K Grid", test_grid, 2048, 2048, 100, 1);
    benchmark("2K Large", test_large, 2048, 2048, 100, 1);
    benchmark("2K Checkerboard", test_check, 2048, 2048, 100, 1);
    benchmark("2K Snake", test_snake, 2048, 2048, 100, 1);

    return 0;
}

