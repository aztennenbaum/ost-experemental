import numpy as np
from PIL import Image, ImageDraw
from scipy.ndimage import label, map_coordinates
from scipy.special import erf
from scipy.sparse import diags, csr_matrix
import sys

class SEConfig:
    def __init__(self, h, w):
        self.threshold_sigma = 5.0
        self.max_stars = 1000
        self.sample_radius = 2
        self.tile_size = 64
        self.img_height, self.img_width = h, w
        self.map_height = int(np.ceil(h / self.tile_size))
        self.map_width = int(np.ceil(w / self.tile_size))

class ImageStats:
    def __init__(self, m, v, p, xi, yi, xg, yg, imax, h, w):
        self.m_img, self.v_img, self.p_img = m, v, p
        self.x_idx, self.y_idx = xi, yi
        self.x_interp, self.y_interp = xg, yg
        self.img_max, self.img_height, self.img_width = imax, h, w

class Star:
    def __init__(self, px, py, val, bgm, bgv, bgp):
        self.Px, self.Py, self.val = px, py, val
        self.bgm, self.bgv, self.bgp = bgm, bgv, bgp

def calc_bg_stats_with_threshold(tile, threshold_sigma):
    if tile.min() == tile.max():
        return float(tile.min()), 0.0, float(tile.min())
    
    if tile.dtype == np.uint8:
        nbins = 256
    elif tile.dtype == np.uint16:
        nbins = 65536
    else:
        nbins = 256
    
    cnt = np.bincount(tile.ravel(), minlength=nbins)
    bv = np.arange(nbins, dtype=np.float64)
    cnt = cnt.astype(np.float64)
    
    msk = cnt > 0
    cnt, bv = cnt[msk], bv[msk]
    
    if len(cnt) == 0:
        return 0.0, 0.0, 0.0
    
    n = len(bv)
    c0, c1, c2 = cnt.copy(), cnt * bv, cnt * bv**2
    s, m, v, t = np.zeros(n), np.zeros(n), np.zeros(n), np.zeros(n)
    
    for i in range(n):
        if i > 0 and bv[i-1] < t[i-1]:
            c0[i] += c0[i-1]
            c1[i] += c1[i-1]
            c2[i] += c2[i-1]
        s[i] = c0[i]
        m[i] = c1[i] / c0[i]
        v[i] = max(c2[i] / c0[i] - m[i]**2 + 1/12, 0.0)
        m[i] += 1/2
        t[i] = m[i] + np.sqrt(v[i]) * threshold_sigma
    
    idx = np.argmax(c0)
    return m[idx], v[idx], t[idx]

def compute_image_stats(img, cfg):
    h, w, mh, mw = img.shape[0], img.shape[1], cfg.map_height, cfg.map_width
    m = np.zeros((mh, mw), dtype=np.float32)
    v = np.zeros((mh, mw), dtype=np.float32)
    p = np.zeros((mh, mw), dtype=np.float32)
    
    yi = np.round(np.linspace(0, h, mh + 1)).astype(int)
    xi = np.round(np.linspace(0, w, mw + 1)).astype(int)
    
    for ty in range(mh):
        for tx in range(mw):
            tile = img[yi[ty]:yi[ty+1], xi[tx]:xi[tx+1]]
            if tile.size == 0: continue
            bm, bv, _ = calc_bg_stats_with_threshold(tile, cfg.threshold_sigma)
            m[ty, tx], v[ty, tx] = bm, bv
            p[ty, tx] = bv / bm if bm > 0 else 0
    
    xg = (xi[1:] + xi[:-1]) / 2.0
    yg = (yi[1:] + yi[:-1]) / 2.0
    return ImageStats(m, v, p, xi, yi, xg, yg, float(np.max(img)), h, w)

def windowed_mvp(px, py, st):
    px0, py0 = px - 1, py - 1
    pxc = np.clip(px0, st.x_interp[0], st.x_interp[-1])
    pyc = np.clip(py0, st.y_interp[0], st.y_interp[-1])
    xi = np.interp(pxc, st.x_interp, np.arange(len(st.x_interp)))
    yi = np.interp(pyc, st.y_interp, np.arange(len(st.y_interp)))
    c = np.array([yi, xi])
    m = map_coordinates(st.m_img, c, order=1, mode='nearest')
    v = map_coordinates(st.v_img, c, order=1, mode='nearest')
    p = map_coordinates(st.p_img, c, order=1, mode='nearest')
    return m, v, p

def star2stats(star):
    P = np.column_stack([star.Py, star.Px])
    val = star.val.astype(np.float64)
    rv = np.column_stack([val, val])
    tot = np.sum(val)
    ctr = np.sum(rv * P, axis=0) / tot
    rc = np.tile(ctr, (len(val), 1))
    cov = ((P - rc) * rv).T @ (P - rc) / tot
    tr = np.trace(cov)
    det = np.linalg.det(cov)
    emin = (tr - np.sqrt(max(tr**2 - 4*det, 0))) / 2
    return ctr, tot, emin

def stats2star(c, r, img, st):
    y, x = c
    h, w = img.shape
    x0 = int(np.clip(np.round(x), 1+r, w-r))
    y0 = int(np.clip(np.round(y), 1+r, h-r))
    xi = np.arange(x0-r, x0+r+1)
    yi = np.arange(y0-r, y0+r+1)
    px = np.repeat(xi, len(yi))
    py = np.tile(yi, len(xi))
    val = img[y0-r-1:y0+r, x0-r-1:x0+r].ravel(order='F')
    bgm, bgv, bgp = windowed_mvp(px.astype(np.float64), py.astype(np.float64), st)
    
    return Star(px, py, val, bgm, bgv, bgp)

def window_centroid(star, st):
    val = star.val.astype(np.float64)
    wval = val - star.bgm
    wvar = np.maximum(val * star.bgp, 0) + star.bgv
    return wval, wvar

def pixelval_and_jacobian(prm, px, py):
    x, y = px - prm[0], py - prm[1]
    tot, rad = prm[2], prm[3]
    s2 = rad * np.sqrt(2)
    
    y1, y2 = (y - 0.5) / s2, (y + 0.5) / s2
    x1, x2 = (x - 0.5) / s2, (x + 0.5) / s2
    
    xv, yv = erf(x1) - erf(x2), erf(y1) - erf(y2)
    
    r1, r2, r3, r4 = np.exp(-x1**2), np.exp(-x2**2), np.exp(-y1**2), np.exp(-y2**2)
    
    r7 = tot / (rad * 2 * np.sqrt(2 * np.pi))
    val = xv * yv * tot / 4
    H = np.column_stack([
        -r7 * yv * (r1 - r2),
        -r7 * xv * (r3 - r4),
        val / tot,
        -np.sqrt(2) * r7 * (xv * (r3*y1 - r4*y2) + yv * (r1*x1 - r2*x2))
    ])
    return val, H

def multiparam_pixval_predictor_decoupled(prm, st, stars):
    if len(prm) <= 1:
        return (csr_matrix((0,0)), np.array([]), np.array([]),
                np.array([]), np.array([]), np.array([]), 0, [])
    
    rad = max(prm[-1], np.sqrt(1.0/12))
    N = min(len(prm)//3, len(stars))
    Hr, Hc, Hd = [], [], []
    pnew, pidx = [], []
    pred, obs, ovar = [], [], []
    snew = []
    roff, coff = 0, 0
    
    for i in range(N):
        star = stars[i]
        px, py = star.Px.astype(np.float64), star.Py.astype(np.float64)
        ov, ovar_part = window_centroid(star, st)
        pv, pH = pixelval_and_jacobian([prm[3*i], prm[3*i+1], prm[3*i+2], rad], px, py)
        
        vidx = (pv < st.img_max) & (pv > st.img_max * np.finfo(float).eps)
        vcnt = np.sum(vidx)
        if vcnt == 0 or np.count_nonzero(ovar_part[vidx]) != vcnt: continue
        
        Hsub, vsub = pH[vidx, :], ovar_part[vidx]
        
        for row in range(vcnt):
            for col in range(3):
                Hr.append(roff + row)
                Hc.append(coff + col)
                Hd.append(Hsub[row, col])
        
        pnew.extend([prm[3*i], prm[3*i+1], prm[3*i+2]])
        pidx.extend([3*i, 3*i+1, 3*i+2])
        snew.append(star)
        pred.extend(pv[vidx])
        obs.extend(ov[vidx])
        ovar.extend(ovar_part[vidx])
        roff += vcnt
        coff += 3
    
    if coff == 0:
        return (csr_matrix((0,0)), np.array([]), np.array([]),
                np.array([]), np.array([]), np.array([]), 0, [])
    
    # Shared PSF radius
    crow = 0
    for i, star in enumerate(snew):
        px, py = star.Px.astype(np.float64), star.Py.astype(np.float64)
        ov, ovar_part = window_centroid(star, st)
        pv, pH = pixelval_and_jacobian([pnew[3*i], pnew[3*i+1], pnew[3*i+2], rad], px, py)
        vidx = (pv < st.img_max) & (pv > st.img_max * np.finfo(float).eps)
        Hsub = pH[vidx, :]
        vcnt = np.sum(vidx)
        for row in range(vcnt):
            Hr.append(crow + row)
            Hc.append(coff)
            Hd.append(Hsub[row, 3])
        crow += vcnt
    
    pnew.append(rad)
    pidx.append(len(prm) - 1)
    H = csr_matrix((Hd, (Hr, Hc)), shape=(roff, coff+1))
    pred, obs, ovar = np.array(pred), np.array(obs), np.array(ovar)
    pnew, pidx = np.array(pnew), np.array(pidx)
    y = obs - pred
    resid = y @ ((1.0/ovar) * y)
    return H, pred, obs, ovar, pnew, pidx, resid, snew

def weighted_least_squares(prm, obs, Rinv, pred, H, alpha):
    y = obs - pred
    if H.shape[0] == 0 or H.shape[1] == 0:
        return prm, None
    
    HtR = H.T @ Rinv
    
    try:
        mat = (HtR @ H).toarray()
        cond = np.linalg.cond(mat)
        print(f"    cond={cond:.4e}")
        P = np.linalg.inv(mat)
        K = P @ HtR
        return prm + alpha * (K @ y), P
    except Exception as e:
        print(f"    inv failed: {e}")
        return prm, None

def extract_stars_and_stats(img, cfg):
    st = compute_image_stats(img, cfg)
    
    tmap = st.m_img + cfg.threshold_sigma * np.sqrt(st.v_img)
    sh, sw = tmap.shape
    dh, dw = img.shape
    yd, xd = np.arange(dh), np.arange(dw)
    sy = (sh-1)/(dh-1) if dh>1 else 0
    sx = (sw-1)/(dw-1) if dw>1 else 0
    ys, xs = yd*sy, xd*sx
    yy, xx = np.meshgrid(ys, xs, indexing='ij')
    c = np.array([yy, xx])
    tmap_lg = map_coordinates(tmap, c, order=1, mode='nearest')
    
    struct = np.array([[0,1,0],[1,1,1],[0,1,0]])
    lbl, nobj = label(img > tmap_lg, structure=struct)
    
    S = []
    
    # Get all pixels for all objects at once
    all_y, all_x = np.where(lbl > 0)
    all_labels = lbl[all_y, all_x]
    all_y1, all_x1 = all_y + 1, all_x + 1
    
    for oid in range(1, nobj+1):
        mask = all_labels == oid
        
        if not np.any(mask): 
            continue
        
        py1, px1 = all_y1[mask], all_x1[mask]
        valid_bounds = np.min(px1) < np.max(px1) and np.min(py1) < np.max(py1)
        
        if not valid_bounds:
            continue
        
        bgm, bgv, bgp = windowed_mvp(px1.astype(np.float64), py1.astype(np.float64), st)
        
        val = img[all_y[mask], all_x[mask]].astype(np.float64) - bgm
        vidx = val > 0
        has_valid = np.sum(vidx) > 0
        
        if not has_valid:
            continue
        
        px_v, py_v, val_v = px1[vidx], py1[vidx], val[vidx]
        bgm_v, bgv_v, bgp_v = bgm[vidx], bgv[vidx], bgp[vidx]
        
        try:
            star = Star(px_v, py_v, val_v, bgm_v, bgv_v, bgp_v)
            ctr, tot, eig = star2stats(star)
            S.append([ctr[0], ctr[1], tot, eig])
        except:
            continue
    
    if len(S) == 0:
        return np.array([]), st, [], np.array([])
    
    S = np.array(S)
    S = S[np.argsort(-S[:,2])][:min(cfg.max_stars, len(S))]
    
    prm, stars = [], []
    for i in range(len(S)):
        try:
            star = stats2star(S[i,:2], cfg.sample_radius, img, st)
            prm.extend([S[i,1], S[i,0], S[i,2]])
            stars.append(star)
        except: continue
    
    if len(stars) == 0:
        return np.array([]), st, [], np.array([])
    
    prm.append(np.sqrt(max(np.mean(S[:len(stars),3]), 1.0/12)))
    return np.array(prm), st, stars, np.array(prm).copy()

def fast_sextractor_decoupled(img):
    cfg = SEConfig(img.shape[0], img.shape[1])
    prm, st, stars, prm0 = extract_stars_and_stats(img, cfg)
    if len(prm) == 0:
        return np.array([]), None, st, np.array([]), np.array([])
    
    P = None
    print(f"init: N={len(prm)//3}")
    
    for it in range(5):
        H, pred, obs, ovar, prm_it, pidx_it, resid, stars_it = \
            multiparam_pixval_predictor_decoupled(prm, st, stars)
        print(f"iter {it}: N={len(prm_it)//3 if len(prm_it)>0 else 0}")
        if len(prm_it) == 0:
            print("  no valid params")
            break
        Rinv = diags(1.0/ovar)
        prm_new, P_new = weighted_least_squares(prm_it, obs, Rinv, pred, H, 1.0)
        if P_new is None:
            print("  wls failed")
            break
        prm, P, stars = prm_new, P_new, stars_it
        print("  ok")
    
    print(f"final: N={(len(prm)-1)//3}")
    H, pred, obs, ovar, prm_f, pidx, resid, stars = \
        multiparam_pixval_predictor_decoupled(prm, st, stars)
    
    if P is not None and len(pidx) > 0:
        P = P[np.ix_(pidx, pidx)]
    
    return prm_f, P, st, prm, prm0

def write_stars_to_file(prm, prm0, P, fname):
    with open(fname, 'w') as f:
        f.write("# star_id x y totalval [unc_x unc_y]\n")
        if len(prm) == 0: return
        N = (len(prm)-1)//3
        f.write(f"# psf_r={prm[-1]:.6f} N={N}\n")
        for i in range(N):
            x, y, tot = prm[3*i], prm[3*i+1], prm[3*i+2]
            if P is not None and 3*i+1 < P.shape[0]:
                ux = np.sqrt(max(P[3*i,3*i], 0))
                uy = np.sqrt(max(P[3*i+1,3*i+1], 0))
                f.write(f"{i+1:4d} {x:12.6f} {y:12.6f} {tot:12.6f} {ux:12.6f} {uy:12.6f}\n")
            else:
                f.write(f"{i+1:4d} {x:12.6f} {y:12.6f} {tot:12.6f}\n")

def draw_stars_on_image(img, prm, output_fname, circle_radius=10, circle_color=(0, 255, 0), line_width=2):
    """
    Draw circles around detected stars and save the annotated image.
    
    Args:
        img: Input image (numpy array)
        prm: Star parameters array from fast_sextractor_decoupled
        output_fname: Output filename for annotated image
        circle_radius: Radius of circles to draw (default: 10)
        circle_color: RGB color tuple (default: green)
        line_width: Width of circle outline (default: 2)
    """
    if len(prm) == 0:
        print("No stars to draw")
        return
    
    # Convert grayscale to RGB if needed
    if len(img.shape) == 2:
        img_rgb = np.stack([img, img, img], axis=2)
    else:
        img_rgb = img.copy()
    
    # Normalize to 8-bit if needed
    if img_rgb.dtype == np.uint16:
        img_rgb = (img_rgb / 256).astype(np.uint8)
    elif img_rgb.dtype != np.uint8:
        img_rgb = ((img_rgb - img_rgb.min()) / (img_rgb.max() - img_rgb.min()) * 255).astype(np.uint8)
    
    # Create PIL image
    pil_img = Image.fromarray(img_rgb)
    draw = ImageDraw.Draw(pil_img)
    
    # Extract star positions
    N = (len(prm) - 1) // 3
    psf_radius = prm[-1]
    
    print(f"\nDrawing {N} stars with circles (radius={circle_radius}px)")
    
    for i in range(N):
        x = prm[3*i]      # x coordinate (1-indexed from image)
        y = prm[3*i+1]    # y coordinate (1-indexed from image)
        tot = prm[3*i+2]  # total intensity
        
        # Convert to 0-indexed pixel coordinates
        px = x - 1
        py = y - 1
        
        # Draw circle
        bbox = [
            px - circle_radius,
            py - circle_radius,
            px + circle_radius,
            py + circle_radius
        ]
        draw.ellipse(bbox, outline=circle_color, width=line_width)
        
        # Optionally draw crosshair at center
        crosshair_size = 3
        draw.line([px - crosshair_size, py, px + crosshair_size, py], 
                  fill=circle_color, width=1)
        draw.line([px, py - crosshair_size, px, py + crosshair_size], 
                  fill=circle_color, width=1)
    
    # Save annotated image
    pil_img.save(output_fname)
    print(f"Saved annotated image: {output_fname}")

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("usage: python fast_sextractor.py <image> <output> [annotated_image]")
        sys.exit(1)
    
    img = np.array(Image.open(sys.argv[1]))
    print(f"img: {sys.argv[1]} {img.shape} {img.dtype} [{img.min()},{img.max()}]")
    
    prm, P, st, dbg, prm0 = fast_sextractor_decoupled(img)
    if len(prm) > 0:
        print(f"extracted {(len(prm)-1)//3} stars, psf_r={prm[-1]:.4f}")
    
    write_stars_to_file(prm, prm0, P, sys.argv[2])
    print(f"wrote: {sys.argv[2]}")
    
    # Draw circles around stars
    annotated_fname = sys.argv[3] if len(sys.argv) > 3 else sys.argv[1].rsplit('.', 1)[0] + '_annotated.png'
    draw_stars_on_image(img, prm, annotated_fname)
