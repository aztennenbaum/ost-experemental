function [ val, H ] = pixelval_and_jacobian( param, Px, Py )
x = Px-param(1);
y = Py-param(2);
totalval = param(3);
psf_radius = param(4); 
y1 = (y - 1/2)/(psf_radius*sqrt(2));
y2 = (y + 1/2)/(psf_radius*sqrt(2));
x1 = (x - 1/2)/(psf_radius*sqrt(2));
x2 = (x + 1/2)/(psf_radius*sqrt(2));
xv = erf(x1) - erf(x2);
yv = erf(y1) - erf(y2);
r1 = exp(-x1.^2);
r2 = exp(-x2.^2);
r3 = exp(-y1.^2);
r4 = exp(-y2.^2);
r7 = totalval/(psf_radius*2*sqrt(2*pi));


val=xv.*yv*totalval/4;
H=[ -r7.*yv.*(r1 - r2),...
    -r7.*xv.*(r3 - r4),...
	 val/totalval,...
    -sqrt(2)*r7.*(xv.*(r3.*y1-r4.*y2)+yv.*(r1.*x1-r2.*x2))];

end