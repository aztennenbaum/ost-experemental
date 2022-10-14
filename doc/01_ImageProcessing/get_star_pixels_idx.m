function [ image_idx ] = get_star_pixels_idx( x_ctr, y_ctr, r, img_width, img_height)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
N=numel(x_ctr);
pixels_per_star=(2*r+1)^2;
image_idx=zeros(N,pixels_per_star);
for i=1:N
    [ x_min, x_max, y_min, y_max ] = windowed_centroid_bounds( x_ctr(i), y_ctr(i), r, img_width, img_height);
    [Px,Py]=meshgrid(x_min:x_max,y_min:y_max);
    [ idx ] = xy2idx( Px, Py, img_height);
    image_idx(i,:)=idx(:);
end
end

