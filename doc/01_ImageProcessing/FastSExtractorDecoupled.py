# fast_sextractor.py - Star extraction via decoupled PSF fitting
import numpy as np
from PIL import Image, ImageDraw
from scipy.ndimage import label, map_coordinates
from scipy.special import erf
from scipy.sparse import diags, csr_matrix
import sys

class SEConfig:
    """Configuration parameters for star extraction."""
    
    def __init__(self, h, w, max_pixel_brightness):
        self.threshold_sigma = 5.0
        self.max_stars = 1000
        self.sample_radius = 5
        self.tile_size = 64
        self.max_pixel_brightness = max_pixel_brightness
        self.map_height = int(np.ceil(h / self.tile_size))
        self.map_width = int(np.ceil(w / self.tile_size))

class ImageStats:
    """Spatially-varying background statistics over image tiles.
    
    Background maps can be interpolated to obtain local estimates
    at arbitrary pixel positions.
    """
    
    def __init__(self, mean, var, poisson, x_centers, y_centers):
        self.mean = mean
        self.var = var
        self.poisson = poisson
        self.x_interp = x_centers
        self.y_interp = y_centers

class Star:
    """Detected star as list of pixels with local background statistics."""
    
    def __init__(self, px, py, val, bg_mean, bg_var, bg_poisson):
        self.px = px
        self.py = py
        self.val = val
        self.bg_mean = bg_mean
        self.bg_var = bg_var
        self.bg_poisson = bg_poisson

def load_image(path):
    """Load image and return normalized grayscale with config.
    
    Returns (img_gray, cfg) where img_gray is uint16 grayscale 2D array
    and cfg.max_pixel_brightness reflects actual data range.
    Returns (None, None) for unsupported formats.
    """
    
    img = np.array(Image.open(path))
    
    # only support uint8 or uint16
    if img.dtype not in [np.uint8, np.uint16]:
        print(f"unsupported data type: {img.dtype}")
        return None, None
    
    # convert to grayscale
    if img.ndim == 3:
        # rgb: r + 2*g + b, promote to next larger type
        r, g, b = img[:,:,0], img[:,:,1], img[:,:,2]
        
        if img.dtype == np.uint8:
            gray = r.astype(np.uint16) + 2*g.astype(np.uint16) + b.astype(np.uint16)
            max_brightness = 255*4
        else:  # uint16
            gray = r.astype(np.uint32) + 2*g.astype(np.uint32) + b.astype(np.uint32)
            max_brightness = 65525*4
    elif img.ndim == 2:
        # grayscale
        if img.dtype == np.uint8:
            gray = img.astype(np.uint16)
            max_brightness = 255
        else:  # uint16
            gray = img
            max_brightness = 65535
    else:
        print(f"unsupported image dimensions: {img.ndim}")
        return None, None
    
    h, w = gray.shape
    cfg = SEConfig(h, w, max_brightness)
    
    return gray, cfg

def compute_background_statistics(tile, sigma_thresh):
    """Robust background estimation via iterative sigma-clipping on histogram."""
    
    if tile.min() == tile.max():
        return float(tile.min()), 0.0, float(tile.min())
    
    nbins = 256 if tile.dtype == np.uint8 else 65536 if tile.dtype == np.uint16 else 256
    hist = np.bincount(tile.ravel(), minlength=nbins).astype(np.float64)
    bins = np.arange(nbins, dtype=np.float64)
    
    mask = hist > 0
    hist, bins = hist[mask], bins[mask]
    
    if len(hist) == 0:
        return 0.0, 0.0, 0.0
    
    n = len(bins)
    c0 = hist.copy()
    c1 = hist * bins
    c2 = hist * bins**2
    
    mu = np.zeros(n)
    var = np.zeros(n)
    thresh = np.zeros(n)
    
    for i in range(n):
        if i > 0 and bins[i-1] < thresh[i-1]:
            c0[i] += c0[i-1]
            c1[i] += c1[i-1]
            c2[i] += c2[i-1]
        
        mu[i] = c1[i] / c0[i]
        var[i] = max(c2[i] / c0[i] - mu[i]**2 + 1/12, 0.0)
        mu[i] += 1/2
        thresh[i] = mu[i] + np.sqrt(var[i]) * sigma_thresh
    
    peak = np.argmax(c0)
    return mu[peak], var[peak], thresh[peak]

def compute_tiled_statistics(img, cfg):
    """Compute background statistics over image tiles."""
    
    h, w = img.shape
    mh, mw = cfg.map_height, cfg.map_width
    
    mean_map = np.zeros((mh, mw), dtype=np.float32)
    var_map = np.zeros((mh, mw), dtype=np.float32)
    poisson_map = np.zeros((mh, mw), dtype=np.float32)
    
    yi = np.round(np.linspace(0, h, mh + 1)).astype(int)
    xi = np.round(np.linspace(0, w, mw + 1)).astype(int)
    
    for ty in range(mh):
        for tx in range(mw):
            tile = img[yi[ty]:yi[ty+1], xi[tx]:xi[tx+1]]
            if tile.size == 0:
                continue
            
            mu, var, _ = compute_background_statistics(tile, cfg.threshold_sigma)
            mean_map[ty, tx] = mu
            var_map[ty, tx] = var
            poisson_map[ty, tx] = var / mu if mu > 0 else 0
    
    xc = (xi[1:] + xi[:-1]) / 2.0
    yc = (yi[1:] + yi[:-1]) / 2.0
    
    return ImageStats(mean_map, var_map, poisson_map, xc, yc)

def interpolate_background(px, py, stats):
    """Bilinear interpolation of background statistics at pixel positions."""
    
    px_clip = np.clip(px, stats.x_interp[0], stats.x_interp[-1])
    py_clip = np.clip(py, stats.y_interp[0], stats.y_interp[-1])
    
    xi = np.interp(px_clip, stats.x_interp, np.arange(len(stats.x_interp)))
    yi = np.interp(py_clip, stats.y_interp, np.arange(len(stats.y_interp)))
    
    coords = np.array([yi, xi])
    
    mu = map_coordinates(stats.mean, coords, order=1, mode='nearest')
    var = map_coordinates(stats.var, coords, order=1, mode='nearest')
    poisson = map_coordinates(stats.poisson, coords, order=1, mode='nearest')
    
    return mu, var, poisson

def compute_star_moments(star):
    """Compute intensity-weighted centroid and minimum eigenvalue."""
    
    P = np.column_stack([star.py, star.px])
    v = star.val.astype(np.float64)
    w = np.column_stack([v, v])
    
    I = np.sum(v)
    c = np.sum(w * P, axis=0) / I
    
    c_rep = np.tile(c, (len(v), 1))
    C = ((P - c_rep) * w).T @ (P - c_rep) / I
    
    tr = np.trace(C)
    det = np.linalg.det(C)
    eig_min = (tr - np.sqrt(max(tr**2 - 4*det, 0))) / 2
    
    return c, I, eig_min

def sample_star_region(centroid, r, img, stats):
    """Extract square region of pixels around centroid."""
    
    y, x = centroid
    h, w = img.shape
    
    xi = int(np.clip(np.round(x), r, w-r-1))
    yi = int(np.clip(np.round(y), r, h-r-1))
    
    x_range = np.arange(xi-r, xi+r+1)
    y_range = np.arange(yi-r, yi+r+1)
    
    px = np.repeat(x_range, len(y_range))
    py = np.tile(y_range, len(x_range))
    
    val = img[yi-r:yi+r+1, xi-r:xi+r+1].ravel(order='F')
    
    mu, var, poisson = interpolate_background(px.astype(np.float64), 
                                              py.astype(np.float64), 
                                              stats)
    
    return Star(px, py, val, mu, var, poisson)

def background_subtract(star):
    """Background-subtracted values and uncertainties."""
    
    v = star.val.astype(np.float64)
    obs = v - star.bg_mean
    var = np.maximum(v * star.bg_poisson, 0) + star.bg_var
    return obs, var

def evaluate_gaussian_psf(params, px, py):
    """Evaluate Gaussian PSF model and Jacobian.
    
    Integrates 2D Gaussian over pixel area using error functions.
    Returns predicted values and derivatives w.r.t. [x, y, I, sigma].
    """
    
    dx = px - params[0]
    dy = py - params[1]
    I = params[2]
    sigma = params[3]
    
    s = sigma * np.sqrt(2)
    
    y1 = (dy - 0.5) / s
    y2 = (dy + 0.5) / s
    x1 = (dx - 0.5) / s
    x2 = (dx + 0.5) / s
    
    ex = erf(x1) - erf(x2)
    ey = erf(y1) - erf(y2)
    
    e1 = np.exp(-x1**2)
    e2 = np.exp(-x2**2)
    e3 = np.exp(-y1**2)
    e4 = np.exp(-y2**2)
    
    norm = I / (sigma * 2 * np.sqrt(2 * np.pi))
    pred = ex * ey * I / 4
    
    J = np.column_stack([
        -norm * ey * (e1 - e2),
        -norm * ex * (e3 - e4),
        pred / I,
        -np.sqrt(2) * norm * (ex * (e3*y1 - e4*y2) + ey * (e1*x1 - e2*x2))
    ])
    
    return pred, J

def build_multistar_model(params, stats, stars, cfg):
    """Build joint observation equation for multiple stars with shared PSF.
    
    Returns sparse Jacobian H, predictions, observations, variances,
    and updated parameter/star lists.
    """
    
    if len(params) <= 1:
        return (csr_matrix((0,0)), np.array([]), np.array([]),
                np.array([]), np.array([]), np.array([]), 0, [])
    
    sigma = max(params[-1], np.sqrt(1.0/12))
    n_stars = min(len(params)//3, len(stars))
    
    Jr, Jc, Jd = [], [], []
    p_upd, p_idx = [], []
    pred_list, obs_list, var_list = [], [], []
    stars_upd = []
    
    row = 0
    col = 0
    
    for i in range(n_stars):
        star = stars[i]
        px = star.px.astype(np.float64)
        py = star.py.astype(np.float64)
        
        obs, var = background_subtract(star)
        
        p = [params[3*i], params[3*i+1], params[3*i+2], sigma]
        pred, J = evaluate_gaussian_psf(p, px, py)
        
        # Exclude saturated pixels
        valid = (pred < cfg.max_pixel_brightness) & (pred > np.sqrt(var))
        n_valid = np.sum(valid)
        
        if n_valid <4 or np.count_nonzero(var[valid]) != n_valid:
            continue
        
        J_sub = J[valid, :]
        
        for r in range(n_valid):
            for c in range(3):
                Jr.append(row + r)
                Jc.append(col + c)
                Jd.append(J_sub[r, c])
        
        p_upd.extend([params[3*i], params[3*i+1], params[3*i+2]])
        p_idx.extend([3*i, 3*i+1, 3*i+2])
        stars_upd.append(star)
        
        pred_list.extend(pred[valid])
        obs_list.extend(obs[valid])
        var_list.extend(var[valid])
        
        row += n_valid
        col += 3
    
    if col == 0:
        return (csr_matrix((0,0)), np.array([]), np.array([]),
                np.array([]), np.array([]), np.array([]), 0, [])
    
    # Add shared PSF parameter column
    cur_row = 0
    for i, star in enumerate(stars_upd):
        px = star.px.astype(np.float64)
        py = star.py.astype(np.float64)
        
        obs, var = background_subtract(star)
        
        p = [p_upd[3*i], p_upd[3*i+1], p_upd[3*i+2], sigma]
        pred, J = evaluate_gaussian_psf(p, px, py)
        
        # Exclude saturated pixels
        valid = (pred < cfg.max_pixel_brightness) & (pred > np.sqrt(var))
        J_sub = J[valid, :]
        n_valid = np.sum(valid)
        
        for r in range(n_valid):
            Jr.append(cur_row + r)
            Jc.append(col)
            Jd.append(J_sub[r, 3])
        cur_row += n_valid
    
    p_upd.append(sigma)
    p_idx.append(len(params) - 1)
    
    H = csr_matrix((Jd, (Jr, Jc)), shape=(row, col+1))
    
    pred = np.array(pred_list)
    obs = np.array(obs_list)
    var = np.array(var_list)
    p_upd = np.array(p_upd)
    p_idx = np.array(p_idx)
    
    resid = obs - pred
    chi2 = resid @ ((1.0/var) * resid)
    
    return H, pred, obs, var, p_upd, p_idx, chi2, stars_upd

# Try to import sparse Cholesky, fall back to dense if unavailable
try:
    from sksparse.cholmod import cholesky
    _HAS_SPARSE_CHOLESKY = True
except ImportError:
    from scipy.linalg import cho_factor, cho_solve
    _HAS_SPARSE_CHOLESKY = False
    print("Warning: sksparse.cholmod not available, falling back to dense Cholesky (slower)")
    print("  Install with: pip install scikit-sparse")

def weighted_least_squares(p, obs, var, pred, H, alpha, compute_cov):
    """One iteration of Gauss-Newton: p_new = p + alpha * K * (obs - pred)."""
    
    HtR = H.T.multiply(1.0 / var)
    b = HtR @ (obs - pred)
    
    if _HAS_SPARSE_CHOLESKY:
        # Sparse Cholesky (fast)
        A = (HtR @ H).tocsc()  # CSC format required for CHOLMOD
        factor = cholesky(A)
        dp = factor(b)
        
        P = None
        if compute_cov:
            P = factor(np.eye(A.shape[0]))
    else:
        # Dense Cholesky (fallback)
        A = (HtR @ H).toarray()  # Convert to dense
        c, low = cho_factor(A)
        dp = cho_solve((c, low), b)
        
        P = None
        if compute_cov:
            P = cho_solve((c, low), np.eye(A.shape[0]))
    
    return p + alpha * dp, P

def extract_stars(img, cfg):
    """Detect stars via threshold and label, compute initial parameters."""
    
    stats = compute_tiled_statistics(img, cfg)
    
    thresh_map = stats.mean + cfg.threshold_sigma * np.sqrt(stats.var)
    mh, mw = thresh_map.shape
    h, w = img.shape
    
    y = np.arange(h)
    x = np.arange(w)
    
    sy = (mh-1)/(h-1) if h > 1 else 0
    sx = (mw-1)/(w-1) if w > 1 else 0
    
    ys = y * sy
    xs = x * sx
    
    yy, xx = np.meshgrid(ys, xs, indexing='ij')
    coords = np.array([yy, xx])
    
    thresh_full = map_coordinates(thresh_map, coords, order=1, mode='nearest')
    
    struct = np.array([[0,1,0],[1,1,1],[0,1,0]])
    labels, n_obj = label(img > thresh_full, structure=struct)
    
    star_props = []
    
    all_y, all_x = np.where(labels > 0)
    all_labels = labels[all_y, all_x]
    
    for obj_id in range(1, n_obj+1):
        mask = all_labels == obj_id
        
        if not np.any(mask):
            continue
        
        py = all_y[mask]
        px = all_x[mask]
        
        if not (np.min(px) < np.max(px) and np.min(py) < np.max(py)):
            continue
        
        mu, var, poisson = interpolate_background(px.astype(np.float64), 
                                                  py.astype(np.float64), 
                                                  stats)
        
        bg_sub = img[py, px].astype(np.float64) - mu
        pos_mask = bg_sub > 0
        
        if np.sum(pos_mask) == 0:
            continue
        
        star = Star(px[pos_mask], py[pos_mask], bg_sub[pos_mask],
                   mu[pos_mask], var[pos_mask], poisson[pos_mask])
        c, I, eig = compute_star_moments(star)
        star_props.append([c[0], c[1], I, eig])
    
    if len(star_props) == 0:
        return np.array([]), stats, [], np.array([])
    
    S = np.array(star_props)
    S = S[np.argsort(-S[:,2])][:min(cfg.max_stars, len(S))]
    
    p0 = []
    stars = []
    
    for i in range(len(S)):
        star = sample_star_region(S[i,:2], cfg.sample_radius, img, stats)
        p0.extend([S[i,1], S[i,0], S[i,2]])
        stars.append(star)
    
    if len(stars) == 0:
        return np.array([]), stats, [], np.array([])
    
    sigma0 = np.sqrt(max(np.mean(S[:len(stars), 3]), 1.0/12))
    p0.append(sigma0)
    
    return np.array(p0), stats, stars, np.array(p0).copy()

def fit_stars(img, cfg, num_iter=5):
    """Fit multiple stars with shared PSF radius via iterative least-squares."""
    
    p, stats, stars, p0 = extract_stars(img, cfg)
    
    if len(p) == 0:
        return np.array([]), None
    
    P = None
    print(f"init: N={len(p)//3}")
    
    for it in range(num_iter):
        H, pred, obs, var, p_it, p_idx, chi2, stars_it = \
            build_multistar_model(p, stats, stars, cfg)
        
        print(f"iter {it}: N={len(p_it)//3 if len(p_it)>0 else 0}")
        
        if len(p_it) == 0:
            print("  no valid params")
            break
        
        # Only compute covariance on the final iteration
        compute_cov = (it == num_iter - 1)
        p_new, P_new = weighted_least_squares(p_it, obs, var, pred, H, 1.0, compute_cov)
        
        if compute_cov and P_new is None:
            print("  wls failed")
            break
        
        p = p_new
        if P_new is not None:
            P = P_new
        stars = stars_it
    
    # Final model evaluation
    H, pred, obs, var, p_f, p_idx, chi2, stars = \
        build_multistar_model(p, stats, stars, cfg)
    
    if P is not None and len(p_idx) > 0:
        P = P[np.ix_(p_idx, p_idx)]
    
    return p_f, P

def write_catalog(params, cov, fname):
    """Write star catalog: id, x, y, intensity, [uncertainties]."""
    
    with open(fname, 'w') as f:
        f.write("# star_id x y totalval [unc_x unc_y]\n")
        
        if len(params) == 0:
            return
        
        n = (len(params)-1)//3
        sigma = params[-1]
        f.write(f"# psf_r={sigma:.6f} N={n}\n")
        
        for i in range(n):
            x = params[3*i]
            y = params[3*i+1]
            I = params[3*i+2]
            
            if cov is not None and 3*i+1 < cov.shape[0]:
                ux = np.sqrt(max(cov[3*i, 3*i], 0))
                uy = np.sqrt(max(cov[3*i+1, 3*i+1], 0))
                f.write(f"{i+1:4d} {x:12.6f} {y:12.6f} {I:12.6f} {ux:12.6f} {uy:12.6f}\n")
            else:
                f.write(f"{i+1:4d} {x:12.6f} {y:12.6f} {I:12.6f}\n")

def to_8bit_rgb(img):
    """Convert to 8-bit RGB for visualization."""
    
    if len(img.shape) == 2:
        # handle different bit depths
        if img.dtype == np.uint16:
            img8 = (img / 256).astype(np.uint8)
        elif img.dtype == np.uint32:
            img8 = (img / 16777216).astype(np.uint8)
        else:
            img8 = img
        rgb = np.stack([img8, img8, img8], axis=2)
    else:
        rgb = img.copy()
    
    if rgb.dtype == np.uint16:
        return (rgb / 256).astype(np.uint8)
    elif rgb.dtype != np.uint8:
        vmin, vmax = rgb.min(), rgb.max()
        return ((rgb - vmin) / (vmax - vmin) * 255).astype(np.uint8)
    
    return rgb

def psf_radius(I, sigma, thresh, var):
    """Radius where Gaussian PSF = thresh*sqrt(var).
    
    2D Gaussian PSF: I/(2*pi*sigma^2) * exp(-r^2/(2*sigma^2))
    Solve: I/(2*pi*sigma^2) * exp(-r^2/(2*sigma^2)) = thresh*sqrt(var)
    r = sigma * sqrt(-2*ln(thresh*sqrt(var)*2*pi*sigma^2/I))
    """
    
    arg = thresh * np.sqrt(var) * 2 * np.pi * sigma**2 / I
    if arg <= 0 or arg >= 1:
        return 3.0
    return sigma * np.sqrt(-2 * np.log(arg))

def draw_stars(img, params, fname, stats=None, cfg=None, color=(0, 255, 0), width=1):
    """Draw circles and crosshairs at star positions."""
    
    if len(params) == 0:
        print("No stars to draw")
        return
    
    rgb = to_8bit_rgb(img)
    pil_img = Image.fromarray(rgb)
    draw = ImageDraw.Draw(pil_img)
    
    n = (len(params) - 1) // 3
    sigma = params[-1]
    
    for i in range(n):
        x = params[3*i]
        y = params[3*i+1]
        I = params[3*i+2]
        
        _, var, _ = interpolate_background(np.array([x]), np.array([y]), stats)
        r = psf_radius(I, sigma, cfg.threshold_sigma, var[0])
        
        draw.line([x - r + 0.5, y + 0.5, x + r + 0.5, y + 0.5], fill=color, width=width)
        draw.line([x + 0.5, y - r + 0.5, x + 0.5, y + r + 0.5], fill=color, width=width) 
    pil_img.save(fname)
    print(f"Saved annotated image: {fname}")

def default_annotated_name(input_fname, output_fname=None):
    """Generate default annotated image filename."""
    if output_fname:
        return output_fname
    return input_fname.rsplit('.', 1)[0] + '_annotated.png'

def main():
    """CLI: python fast_sextractor.py <image> <output> [annotated_image]"""
    
    if len(sys.argv) < 3:
        print("usage: python fast_sextractor.py <image> <output> [annotated_image]")
        sys.exit(1)
    
    img_fname = sys.argv[1]
    cat_fname = sys.argv[2]
    ann_fname = sys.argv[3] if len(sys.argv) > 3 else default_annotated_name(img_fname)
    
    img, cfg = load_image(img_fname)
    
    if img is None:
        sys.exit(1)
    
    print(f"img: {img_fname} {img.shape} {img.dtype} [{img.min()},{img.max()}] max_brightness={cfg.max_pixel_brightness}")
    
    params, cov = fit_stars(img, cfg)
    
    if len(params) > 0:
        n = (len(params)-1)//3
        sigma = params[-1]
        print(f"extracted {n} stars, psf_r={sigma:.4f}")
    
    write_catalog(params, cov, cat_fname)
    print(f"wrote: {cat_fname}")
    
    draw_stars(img, params, ann_fname, stats=compute_tiled_statistics(img, cfg), cfg=cfg)


if __name__ == '__main__':
    main()
