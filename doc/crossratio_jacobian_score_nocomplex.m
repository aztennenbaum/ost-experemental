function [ out ] = crossratio_jacobian_score_nocomplex( in )
%derivation
% in=rand(1,8)
% out1=crossratio_jacobian_nocomplex(in);
% out1=det(out1*(out1');
% crossratio_jacobian_det_nocomplex(in)-out1
a=in(:,1:2);
b=in(:,3:4);
c=in(:,5:6);
d=in(:,7:8);
out1=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
a=in(:,1:2);
c=in(:,3:4);
b=in(:,5:6);
d=in(:,7:8);
out2=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
a=in(:,1:2);
d=in(:,3:4);
c=in(:,5:6);
b=in(:,7:8);
out3=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
a=in(:,1:2);
b=in(:,3:4);
d=in(:,5:6);
c=in(:,7:8);
out4=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
a=in(:,1:2);
d=in(:,3:4);
b=in(:,5:6);
c=in(:,7:8);
out5=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
a=in(:,1:2);
c=in(:,3:4);
d=in(:,5:6);
b=in(:,7:8);
out6=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
out=[out1,out2,out3,out4,out5,out6];
end

