function [ I_config ] = default_SE_config( I )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

ConcavityThreshold=0.15;%minimum concavity of the central 3x3 pixel window.
                      %0.1-0.25 was experementally determined to reject
                      %image artifacts over a large variety of source
                      %images. If this is too close to zero, the psf_width
                      %solution may diverge
                      
TileSize = 64;  %according to SExtractor documentation 32-256 works well.
BrightestN = 10;%use the N brightest stars that meet all of the criteria. 10 seems to be the best
r = 2; %pixel sample radui - sample pixels in a d^2 square centered at the
       %middle of the threshold pixels, where d=2*r+1
img_height = size(I,1);
img_width  = size(I,2);

map_height = ceil(size(I,1)/TileSize);
map_width  = ceil(size(I,2)/TileSize);

threshold_sigma = 5;
max_star_size   = 20;
min_star_size   = 2;
 
I_config=struct('ConcavityThreshold',ConcavityThreshold,...
	           'TileSize',TileSize,...
			   'BrightestN',BrightestN,...
               'r',r,...
               'threshold_sigma',single(threshold_sigma),...
               'max_stars',uint64(BrightestN),...
               'sample_radius',uint64(r),...
			   'img_height',uint64(img_height),...
			   'img_width',uint64(img_width),...
               'map_height',uint64(map_height),...
               'map_width',uint64(map_width),...
               'max_star_size',uint64(max_star_size),...
               'min_star_size',uint64(min_star_size));


end

