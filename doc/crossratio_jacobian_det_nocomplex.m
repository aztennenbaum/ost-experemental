function [ out ] = crossratio_jacobian_det_nocomplex( in )
%derivation
% in=rand(1,8)
% out1=crossratio_jacobian_nocomplex(in);
% out1=det(out1*(out1'));
% crossratio_jacobian_det_nocomplex(in)-out1
a=in(:,1:2);
b=in(:,3:4);
c=in(:,5:6);
d=in(:,7:8);
%cr=crossratio_nocomplex(in);
%out=sum([cmult(cr,cinv(a-c)-cinv(a-d)),cmult(cr,cinv(b-d)-cinv(b-c)),cmult(cr,cinv(b-c)-cinv(a-c)),cmult(cr,cinv(a-d)-cinv(b-d))].^2,2).^2;
out=(sum(crossratio_nocomplex(in).^2,2).*sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2)).^2;
end

