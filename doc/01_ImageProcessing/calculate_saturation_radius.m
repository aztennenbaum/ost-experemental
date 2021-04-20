function [ sr ] = calculate_saturation_radius(totalval, psf_radius,maxval )
%UNTITLED15 Summary of this function goes here
%   Detailed explanation goes here

	sr=psf_radius*sqrt(max(2*log(totalval/(2*maxval*psf_radius^2*pi)),0));


end

