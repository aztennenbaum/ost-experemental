function [ M ] = stupid_wahba2( x, y )
%UNTITLED8 Summary of this function goes here
%   Detailed explanation goes here
% [u,s,v]=svd([x-y (x+y)*1i 1+x.*y (1-x.*y)*1i])
x=stereoprojinv(transpose(x),1);
y=stereoprojinv(transpose(y),1);
[u,s,v]=svd(y*x')

M=q2mobius(a2q(u*[1 0 0;0 1 0;0 0 det(u)*det(v)]*v'))
M=M/sqrt(det(M))
end

