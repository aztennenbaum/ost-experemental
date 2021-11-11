function [ out ] = crossratio_nocomplex( in )
a=in(:,1:2);
b=in(:,3:4);
c=in(:,5:6);
d=in(:,7:8);

out=cmult(cmult(a-c,b-d),cinv(cmult(b-c,a-d)));
end

