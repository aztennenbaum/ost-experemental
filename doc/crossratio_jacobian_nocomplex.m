function [ out ] = crossratio_jacobian_nocomplex( in )
%derivation
% syms x1 y1 x2 y2 x3 y3 x4 y4
% assume([x1 y1 x2 y2 x3 y3 x4 y4],'real')
% in=[x1 y1 x2 y2 x3 y3 x4 y4]
% simplify(jacobian(crossratio_nocomplex(in),in)-crossratio_jacobian_nocomplex(in))
a=in(:,1:2);
b=in(:,3:4);
c=in(:,5:6);
d=in(:,7:8);
cr=crossratio_nocomplex(in);
[out_real,out_imag]=csplit([cmult(cr,cinv(a-c)-cinv(a-d)),cmult(cr,cinv(b-d)-cinv(b-c)),cmult(cr,cinv(b-c)-cinv(a-c)),cmult(cr,cinv(a-d)-cinv(b-d))]);
out=cjoin(cjoin(out_real,-out_imag)',cjoin(out_imag,out_real)')';
end

