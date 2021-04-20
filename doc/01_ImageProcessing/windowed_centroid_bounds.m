function [ x_min, x_max, y_min, y_max ] = windowed_centroid_bounds( x_ctr, y_ctr, r, img_width, img_height)
%WINDOWED_CENTROID_BOUNDS Summary of this function goes here
%   Detailed explanation goes here
% 	
	center_pixel_x = max(min(img_width-r,round(x_ctr)),1+r);
	center_pixel_y = max(min(img_height-r,round(y_ctr)),1+r);
	
	x_min=(center_pixel_x-r);
	x_max=(center_pixel_x+r);
	y_min=(center_pixel_y-r);
	y_max=(center_pixel_y+r);


end

