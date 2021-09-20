function [ val ] = pixelval( params,  Px, Py )
x = Px-params(1);
y = Py-params(2);
totalval = params(3);
psf_radius = params(4); 
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here
y1 = (y - 1/2)./(psf_radius*sqrt(2));
y2 = (y + 1/2)./(psf_radius*sqrt(2));
x1 = (x - 1/2)./(psf_radius*sqrt(2));
x2 = (x + 1/2)./(psf_radius*sqrt(2));
xv = erf(x1) - erf(x2);
yv = erf(y1) - erf(y2);

val=xv.*yv*totalval/4;
end

