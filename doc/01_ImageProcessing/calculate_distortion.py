#!/usr/bin/env python3
# calculate_distortion.py - Fit Brown distortion with Blender frame support
# Usage: python calculate_distortion.py [--solve-rotation] undistorted/ distorted/

import numpy as np
import sys
import os
from pathlib import Path
from scipy.optimize import least_squares
from scipy.spatial.transform import Rotation
from FastSExtractorDecoupled import load_image, fit_stars

# Camera intrinsics
fx = 6704.167728825494
fy = 6705.612571275518
cx = 639.3394190784027
cy = 479.8844185963926
img_width = 1280
img_height = 960

# Expected distortion parameters (for comparison)
k1_expected = 0.23136142375486132
k2_expected = 20.70582965843243
k3_expected = -922.230162629575
p1_expected = -0.0008097172058476805
p2_expected = -0.0008789413533530643

def params_to_stars(params, cov):
    if len(params) == 0:
        return np.array([])
    n = (len(params) - 1) // 3
    stars = []
    for i in range(n):
        x, y, I = params[3*i], params[3*i+1], params[3*i+2]
        if cov is not None and 3*i+1 < cov.shape[0]:
            ux = np.sqrt(max(cov[3*i, 3*i], 0))
            uy = np.sqrt(max(cov[3*i+1, 3*i+1], 0))
        else:
            ux = uy = 1.0
        stars.append([x, y, I, ux, uy])
    return np.array(stars)

def match_bidirectional(u_stars, d_stars):
    u_pos, d_pos = u_stars[:, :2], d_stars[:, :2]
    nu, nd = len(u_pos), len(d_pos)
    
    u_to_d = np.zeros(nu, dtype=int)
    for i in range(nu):
        u_to_d[i] = np.argmin(np.sum((d_pos - u_pos[i])**2, axis=1))
    
    d_to_u = np.zeros(nd, dtype=int)
    for j in range(nd):
        d_to_u[j] = np.argmin(np.sum((u_pos - d_pos[j])**2, axis=1))
    
    u_idx = [i for i in range(nu) if d_to_u[u_to_d[i]] == i]
    d_idx = [u_to_d[i] for i in u_idx]
    
    return np.array(u_idx), np.array(d_idx)

def ray_to_pixel(rays):
    x = rays[:, 0] / rays[:, 2]
    y = rays[:, 1] / rays[:, 2]
    px = x * fx + cx
    py = y * fy + cy
    return px, py

def brown_distort(x, y, k1, k2, k3, p1, p2):
    r2 = x*x + y*y
    r4 = r2*r2
    r6 = r2*r4
    radial = k1*r2 + k2*r4 + k3*r6
    xd = x * (1 + radial) + (2*p1*x*y + p2*(r2 + 2*x*x))
    yd = y * (1 + radial) + (p1*(r2 + 2*y*y) + 2*p2*x*y)
    return xd, yd

def compute_boresight_displacement(R):
    """Compute displacement at boresight (image center) due to rotation."""
    # Boresight is at the principal point
    boresight_px = np.array([[cx, cy]])
    
    # Convert to normalized coordinates
    x = (boresight_px[:, 0] - cx) / fx
    y = (boresight_px[:, 1] - cy) / fy
    
    # Create ray (should be [0, 0, 1] at boresight)
    ray = np.array([[x[0], y[0], 1.0]])
    ray = ray / np.linalg.norm(ray, axis=1, keepdims=True)
    
    # Apply rotation
    ray_rot = (R.as_matrix() @ ray.T).T
    
    # Project back to pixel coordinates
    px_rot, py_rot = ray_to_pixel(ray_rot)
    
    # Compute displacement
    dx = px_rot[0] - boresight_px[0, 0]
    dy = py_rot[0] - boresight_px[0, 1]
    
    return dx, dy

def get_sorted_image_files(directory):
    """Get all image files from directory, sorted alphabetically."""
    valid_extensions = {'.tif', '.tiff', '.png', '.jpg', '.jpeg', '.bmp'}
    path = Path(directory)
    
    if not path.is_dir():
        raise ValueError(f"Not a directory: {directory}")
    
    files = [f for f in path.iterdir() 
             if f.is_file() and f.suffix.lower() in valid_extensions]
    
    return sorted(files)

def extract_stars_from_image_pair(u_path, d_path):
    """Extract stars from a single undistorted/distorted image pair."""
    try:
        img_u, cfg_u = load_image(str(u_path))
        params_u, cov_u = fit_stars(img_u, cfg_u)
        
        img_d, cfg_d = load_image(str(d_path))
        params_d, cov_d = fit_stars(img_d, cfg_d)
        
        if params_u is None or params_d is None or len(params_u) == 0 or len(params_d) == 0:
            return None, None
        
        stars_u = params_to_stars(params_u, cov_u)
        stars_d = params_to_stars(params_d, cov_d)
        
        return stars_u, stars_d
    except Exception as e:
        print(f"       error: {e}")
        return None, None

def fit_distortion_only(u_px, u_py, d_px, d_py, u_ux, u_uy, d_ux, d_uy):
    """Fit distortion parameters only (no rotation)."""
    var_u = (u_ux**2 + u_uy**2) / 2
    var_d = (d_ux**2 + d_uy**2) / 2
    w = np.sqrt(1.0 / (var_u + var_d + 1e-10))
    
    def residuals(p):
        k1, k2, k3, p1, p2 = p
        
        x = (u_px - cx) / fx
        y = (u_py - cy) / fy
        
        # Apply distortion
        xd, yd = brown_distort(x, y, k1, k2, k3, p1, p2)
        
        # Convert to pixels
        px = xd * fx + cx
        py = yd * fy + cy
        
        return np.concatenate([w * (px - d_px), w * (py - d_py)])
    
    p0 = [0, 0, 0, 0, 0]
    result = least_squares(residuals, p0, verbose=0, max_nfev=200)
    
    return result.x

def fit_distortion_with_rotations(image_pairs_data, num_pairs):
    """
    Fit distortion with per-image rotations.
    
    image_pairs_data: list of tuples (u_px, u_py, d_px, d_py, u_ux, u_uy, d_ux, d_uy)
    num_pairs: number of image pairs
    """
    # Count total points
    total_points = sum(len(data[0]) for data in image_pairs_data)
    
    print(f"  fitting {5} distortion params + {3 * num_pairs} rotation params = {5 + 3*num_pairs} total params")
    print(f"  using {total_points} matched star pairs from {num_pairs} images")
    
    def residuals(p):
        # First 5 params are distortion (shared across all images)
        k1, k2, k3, p1, p2 = p[:5]
        
        # Remaining params are rotation vectors (3 per image)
        rotation_params = p[5:].reshape(num_pairs, 3)
        
        all_residuals = []
        
        for pair_idx, (u_px, u_py, d_px, d_py, u_ux, u_uy, d_ux, d_uy) in enumerate(image_pairs_data):
            if len(u_px) == 0:
                continue
            
            var_u = (u_ux**2 + u_uy**2) / 2
            var_d = (d_ux**2 + d_uy**2) / 2
            w = np.sqrt(1.0 / (var_u + var_d + 1e-10))
            
            x = (u_px - cx) / fx
            y = (u_py - cy) / fy
            
            # Apply distortion
            xd, yd = brown_distort(x, y, k1, k2, k3, p1, p2)
            
            # Convert to rays
            rays_dist = np.column_stack([xd, yd, np.ones_like(xd)])
            norms = np.linalg.norm(rays_dist, axis=1, keepdims=True)
            rays_dist = rays_dist / norms
            
            # Apply rotation for this specific image pair
            rx, ry, rz = rotation_params[pair_idx]
            R = Rotation.from_rotvec([rx, ry, rz]).as_matrix()
            rays_rot = (R @ rays_dist.T).T
            
            # Project to pixels
            px, py = ray_to_pixel(rays_rot)
            
            all_residuals.extend(w * (px - d_px))
            all_residuals.extend(w * (py - d_py))
        
        return np.array(all_residuals)
    
    # Initial guess: zero distortion, zero rotations
    p0 = np.zeros(5 + 3 * num_pairs)
    
    result = least_squares(residuals, p0, verbose=2, max_nfev=500)
    
    k1, k2, k3, p1, p2 = result.x[:5]
    rotation_params = result.x[5:].reshape(num_pairs, 3)
    rotations = [Rotation.from_rotvec(rv) for rv in rotation_params]
    
    return k1, k2, k3, p1, p2, rotations

def format_cpp_float(value):
    """Format a float value for C++ with proper precision and 'f' suffix."""
    # Format with 20 decimal places to match the style
    formatted = f"{value:.20f}"
    # Remove trailing zeros after the decimal point, but keep at least 2 digits
    parts = formatted.split('.')
    if len(parts) == 2:
        integer_part = parts[0]
        decimal_part = parts[1].rstrip('0')
        if len(decimal_part) < 2:
            decimal_part = decimal_part.ljust(2, '0')
        formatted = f"{integer_part}.{decimal_part}"
    return formatted + 'f'

def print_cpp_coefficients(k1, k2, k3, p1, p2):
    """Print distortion coefficients in C++ format."""
    print("\n" + "="*70)
    print("C++ CODE (ready to paste):")
    print("="*70)
    print()
    print("    // Brown distortion coefficients")
    print(f"    constexpr float k1 = {format_cpp_float(k1)};")
    print(f"    constexpr float k2 = {format_cpp_float(k2)};")
    print(f"    constexpr float k3 = {format_cpp_float(k3)};")
    print(f"    constexpr float p1 = {format_cpp_float(p1)};")
    print(f"    constexpr float p2 = {format_cpp_float(p2)};")
    print()
    print("="*70)

def main():
    solve_rotation = False
    args = sys.argv[1:]
    
    if '--solve-rotation' in args:
        solve_rotation = True
        args.remove('--solve-rotation')
    
    if len(args) != 2:
        print("usage: python calculate_distortion.py [--solve-rotation] undistorted/ distorted/")
        return 1
    
    undistorted_dir = args[0]
    distorted_dir = args[1]
    
    # Get sorted image files
    print(f"scanning directories...")
    try:
        u_files = get_sorted_image_files(undistorted_dir)
        d_files = get_sorted_image_files(distorted_dir)
    except Exception as e:
        print(f"error: {e}")
        return 1
    
    if len(u_files) == 0 or len(d_files) == 0:
        print(f"error: no images found (undistorted: {len(u_files)}, distorted: {len(d_files)})")
        return 1
    
    if len(u_files) != len(d_files):
        print(f"warning: different number of images (undistorted: {len(u_files)}, distorted: {len(d_files)})")
        print(f"         will use first {min(len(u_files), len(d_files))} pairs")
    
    # Use matching number of files
    num_pairs = min(len(u_files), len(d_files))
    u_files = u_files[:num_pairs]
    d_files = d_files[:num_pairs]
    
    print(f"\nprocessing {num_pairs} image pairs")
    
    # Extract stars from each image pair
    print(f"\nextracting and matching stars from image pairs...")
    
    if solve_rotation:
        # For rotation mode: keep pairs separate
        image_pairs_data = []
        pair_info = []
        
        for i, (u_path, d_path) in enumerate(zip(u_files, d_files)):
            print(f"  [{i+1}/{num_pairs}] {u_path.name} <-> {d_path.name}")
            
            stars_u, stars_d = extract_stars_from_image_pair(u_path, d_path)
            
            if stars_u is None or stars_d is None:
                print(f"       skipping (extraction failed)")
                continue
            
            print(f"       found {len(stars_u)} undistorted, {len(stars_d)} distorted stars")
            
            # Match stars for this pair
            u_idx, d_idx = match_bidirectional(stars_u, stars_d)
            print(f"       matched {len(u_idx)} stars")
            
            if len(u_idx) < 3:
                print(f"       skipping (too few matches)")
                continue
            
            u_px, u_py = stars_u[u_idx, 0], stars_u[u_idx, 1]
            d_px, d_py = stars_d[d_idx, 0], stars_d[d_idx, 1]
            u_ux, u_uy = stars_u[u_idx, 3], stars_u[u_idx, 4]
            d_ux, d_uy = stars_d[d_idx, 3], stars_d[d_idx, 4]
            
            image_pairs_data.append((u_px, u_py, d_px, d_py, u_ux, u_uy, d_ux, d_uy))
            pair_info.append((u_path.name, d_path.name))
        
        if len(image_pairs_data) == 0:
            print("error: no valid image pairs")
            return 1
        
        print(f"\nfitting distortion with per-image rotations...")
        k1, k2, k3, p1, p2, rotations = fit_distortion_with_rotations(
            image_pairs_data, len(image_pairs_data)
        )
        
    else:
        # For no-rotation mode: combine all stars
        all_u_stars = []
        all_d_stars = []
        
        for i, (u_path, d_path) in enumerate(zip(u_files, d_files)):
            print(f"  [{i+1}/{num_pairs}] {u_path.name} <-> {d_path.name}")
            
            stars_u, stars_d = extract_stars_from_image_pair(u_path, d_path)
            
            if stars_u is None or stars_d is None:
                print(f"       skipping (extraction failed)")
                continue
            
            print(f"       found {len(stars_u)} undistorted, {len(stars_d)} distorted stars")
            all_u_stars.append(stars_u)
            all_d_stars.append(stars_d)
        
        if len(all_u_stars) == 0:
            print("error: no stars extracted")
            return 1
        
        # Combine all stars
        u_stars = np.vstack(all_u_stars)
        d_stars = np.vstack(all_d_stars)
        
        print(f"\ntotal: {len(u_stars)} undistorted, {len(d_stars)} distorted stars")
        print(f"matching stars...")
        
        u_idx, d_idx = match_bidirectional(u_stars, d_stars)
        print(f"matched {len(u_idx)} stars")
        
        if len(u_idx) < 5:
            print(f"error: insufficient matches ({len(u_idx)}), need at least 5")
            return 1
        
        u_px, u_py = u_stars[u_idx, 0], u_stars[u_idx, 1]
        d_px, d_py = d_stars[d_idx, 0], d_stars[d_idx, 1]
        u_ux, u_uy = u_stars[u_idx, 3], u_stars[u_idx, 4]
        d_ux, d_uy = d_stars[d_idx, 3], d_stars[d_idx, 4]
        
        print(f"\nfitting distortion without rotation...")
        result = fit_distortion_only(u_px, u_py, d_px, d_py, u_ux, u_uy, d_ux, d_uy)
        k1, k2, k3, p1, p2 = result
        rotations = None
    
    # Evaluate fit quality
    print(f"\nevaluating fit quality...")
    
    if solve_rotation:
        # Evaluate on each image pair separately
        all_errors = []
        
        for pair_idx, (u_px, u_py, d_px, d_py, u_ux, u_uy, d_ux, d_uy) in enumerate(image_pairs_data):
            x = (u_px - cx) / fx
            y = (u_py - cy) / fy
            
            xd, yd = brown_distort(x, y, k1, k2, k3, p1, p2)
            
            rays_dist = np.column_stack([xd, yd, np.ones_like(xd)])
            rays_dist = rays_dist / np.linalg.norm(rays_dist, axis=1, keepdims=True)
            rays_rot = (rotations[pair_idx].as_matrix() @ rays_dist.T).T
            px, py = ray_to_pixel(rays_rot)
            
            err = np.sqrt((px - d_px)**2 + (py - d_py)**2)
            all_errors.extend(err)
            
            print(f"  pair {pair_idx+1} ({pair_info[pair_idx][0]}): "
                  f"rms={np.sqrt(np.mean(err**2)):.4f} px, "
                  f"median={np.median(err):.4f} px, "
                  f"n={len(err)}")
        
        err = np.array(all_errors)
    else:
        x = (u_px - cx) / fx
        y = (u_py - cy) / fy
        
        xd, yd = brown_distort(x, y, k1, k2, k3, p1, p2)
        px = xd * fx + cx
        py = yd * fy + cy
        
        err = np.sqrt((px - d_px)**2 + (py - d_py)**2)
    
    print(f"\noverall fit quality:")
    print(f"  rms: {np.sqrt(np.mean(err**2)):.4f} px")
    print(f"  median: {np.median(err):.4f} px")
    print(f"  95th percentile: {np.percentile(err, 95):.4f} px")
    print(f"  max: {np.max(err):.4f} px")
    
    print(f"\ndistortion parameters:")
    print(f"  k1 = {k1:.15e}")
    print(f"  k2 = {k2:.15e}")
    print(f"  k3 = {k3:.15e}")
    print(f"  p1 = {p1:.15e}")
    print(f"  p2 = {p2:.15e}")
    
    print(f"\nratio to expected:")
    print(f"  k1/k1_expected = {k1/k1_expected:.6f}")
    print(f"  k2/k2_expected = {k2/k2_expected:.6f}")
    print(f"  k3/k3_expected = {k3/k3_expected:.6f}")
    print(f"  p1/p1_expected = {p1/p1_expected:.6f}")
    print(f"  p2/p2_expected = {p2/p2_expected:.6f}")
    
    if solve_rotation:
        print(f"\nrelative rotations:")
        for pair_idx, R in enumerate(rotations):
            rvec = R.as_rotvec()
            angle_deg = np.linalg.norm(rvec) * 180 / np.pi
            dx, dy = compute_boresight_displacement(R)
            
            print(f"  pair {pair_idx+1} ({pair_info[pair_idx][0]}):")
            print(f"    angle = {angle_deg:.6f} deg")
            print(f"    boresight displacement: dx = {dx:.4f} px, dy = {dy:.4f} px")
            print(f"    rodriguez = [{rvec[0]:.6e}, {rvec[1]:.6e}, {rvec[2]:.6e}]")
    
    # Print C++ formatted output
    print_cpp_coefficients(k1, k2, k3, p1, p2)
    
    return 0

if __name__ == "__main__":
    sys.exit(main())

