function [ I_config ] = default_SE_config( I )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

ConcavityThreshold=0.15;%minimum concavity of the central 3x3 pixel window.
                        %0.1-0.25 was experementally determined to reject
                        %image artifacts over a large variety of source
                        %images. If this is too close to zero, the psf_width
                        %solution may diverge

max_stars = 70;%use the N brightest stars that meet all of the criteria. 10 seems to be the best
                % NOTE: we do an online insertion sort of O(MAX_STARS^2) to get the top N stars
                % If MAX_STARS is >1000, you may need to swap this with nth_element (or the c equivalent)
                % the cost of this is you will need enough memory to remember every star, which could be
                % as much as IMAGE_SIZE/2
                % max_stars=70;

sample_radius = 2; %pixel sample radui - sample pixels in a d^2 square centered at the
                   %middle of the threshold pixels, where d=2*sample_radius+1
img_height = size(I,1);
img_width  = size(I,2);

TileSize = 64;  %according to SExtractor documentation 32-256 works well.
map_height = ceil(size(I,1)/TileSize);
map_width  = ceil(size(I,2)/TileSize);

threshold_sigma = 5;

max_star_size   = 20;% Throw out any object w/ width or height that's more than this
min_star_size   = 2;% Throw out any object w/ width or height that's less than this
                    % Recomended to set this to 2 in order to filter out noise and dead pixels

% Set a maximum pixel brightness in order to filter out saturated pixels so that 
max_pixel_brightness=255;

I_config=struct('threshold_sigma',single(threshold_sigma),...
                'max_stars',uint64(max_stars),...
                'sample_radius',uint64(sample_radius),...
                'img_height',uint64(img_height),...
                'img_width',uint64(img_width),...
                'map_height',uint64(map_height),...
                'map_width',uint64(map_width),...
                'max_star_size',uint64(max_star_size),...
                'min_star_size',uint64(min_star_size),...
                'max_pixel_brightness',max_pixel_brightness);

end

