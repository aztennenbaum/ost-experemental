function [ val ] = pixelval( param,  Px, Py )
x = Px-param(1);
y = Py-param(2);
totalval = param(3);
psf_radius = param(4); 
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

