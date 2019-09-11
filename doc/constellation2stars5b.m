function [ s ] = constellation2stars5b( c )
%UNTITLED17 Summary of this function goes here
%   Detailed explanation goes here
% syms x1 y1 z1;
% assume([x1 y1 z1],'real');
% assumeAlso(x1^2+y1^2+z1^2==1)
% simplify(2/sum([x1/(z1-1) y1/(z1-1) 1].^2))
% z1 - 1
c=[c(1) cumprod([-c(2) -1 c(3:end)])];
c=[real(c);imag(c);ones(size(c))];
c=c*2./sum(c.^2,1);
c(3,:)=1-c(3,:);
s=q2a(normalize(qinv(rotvec2(transpose(c(:,1))))))*c(:,2:end);
end

