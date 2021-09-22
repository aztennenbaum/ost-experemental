function [ stars ] = params_and_image_to_stars( params, I, r )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    stars = struct('Pwx',{},'Pwy',{},'val',{});
    N=floor(numel(params)/3);
    for i=1:N
        centroid   = params(3*(i-1)+[2 1]);
        stars(i)=params2star(centroid ,r, I);
    end
end
