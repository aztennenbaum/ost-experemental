function [ sr ] = worst_case_saturation_radius(totalval, maxval )
%UNTITLED16 Summary of this function goes here
%   Detailed explanation goes here

psf_radius=exp(-1/2)*sqrt(2*totalval/(maxval*pi))/2;
sr=calculate_saturation_radius(totalval, psf_radius,maxval );

end

