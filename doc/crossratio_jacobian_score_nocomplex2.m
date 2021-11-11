function [ sensitivity , cr] = crossratio_jacobian_score_nocomplex2( in )
%cr(:,1:3)==1./cr(:,4:6)
%sensitivity(:,1:3)==sensitivity(:,4:6)
%derivation
% in=rand(1,8)
% sensitivity1=crossratio_jacobian_nocomplex(in);
% sensitivity1=det(sensitivity1*(sensitivity1');
% crossratio_jacobian_det_nocomplex(in)-sensitivity1
a=in(:,1:2);
b=in(:,3:4);
c=in(:,5:6);
d=in(:,7:8);
cr1=sum(crossratio_nocomplex([a b c d]).^2,2);
sensitivity1=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
a=in(:,1:2);
c=in(:,3:4);
b=in(:,5:6);
d=in(:,7:8);
cr2=sum(crossratio_nocomplex([a b c d]).^2,2);
sensitivity2=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
a=in(:,1:2);
c=in(:,3:4);
d=in(:,5:6);
b=in(:,7:8);
cr3=sum(crossratio_nocomplex([a b c d]).^2,2);
sensitivity3=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
a=in(:,1:2);
b=in(:,3:4);
d=in(:,5:6);
c=in(:,7:8);
cr4=sum(crossratio_nocomplex([a b c d]).^2,2);
sensitivity4=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
a=in(:,1:2);
d=in(:,3:4);
b=in(:,5:6);
c=in(:,7:8);
cr5=sum(crossratio_nocomplex([a b c d]).^2,2);
sensitivity5=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
a=in(:,1:2);
d=in(:,3:4);
c=in(:,5:6);
b=in(:,7:8);
cr6=sum(crossratio_nocomplex([a b c d]).^2,2);
sensitivity6=sum([cinv(a-c)-cinv(a-d),cinv(b-d)-cinv(b-c),cinv(b-c)-cinv(a-c),cinv(a-d)-cinv(b-d)].^2,2);
sensitivity=[sensitivity1,sensitivity2,sensitivity3,sensitivity4,sensitivity5,sensitivity6];
cr=[cr1,cr2,cr3,cr4,cr5,cr6];
end

