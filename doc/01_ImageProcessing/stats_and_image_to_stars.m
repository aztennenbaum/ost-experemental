function [ stars ] = stats_and_image_to_stars( S_stats, I, r )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    stars = struct('Px',{},'Py',{},'val',{});
    N=size(S_stats,1);
    for i=1:N
        centroid = S_stats(i,1:2);
        stars(i) = stats2star(centroid ,r, I);
    end
end

