function [ out ] = constellation_normalized_sensitivity( in )
%calculate normalized sesitivity assuming all stars have equal noise
%this is equal to
%sqrt(crossratio_jacobian_det_nocomplex(in))/sum(crossratio_nocomplex(in).^2,2))
a=in(:,1:2);
b=in(:,3:4);
c=in(:,5:6);
d=in(:,7:8);

out=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
end

