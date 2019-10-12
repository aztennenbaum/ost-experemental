function [ M ] = stupid_wahba( x, y )
%UNTITLED8 Summary of this function goes here
%   Detailed explanation goes here
% [u,s,v]=svd([x-y (x+y)*1i 1+x.*y (1-x.*y)*1i])
[v,s]=eig(-[x-y (x+y)*1i 1+x.*y (1-x.*y)*1i]'*([x-y (x+y)*1i 1+x.*y (1-x.*y)*1i]));
M=[v(1,4)+1i*v(2,4) v(3,4)+1i*v(4,4);-v(3,4)+1i*v(4,4) v(1,4)-1i*v(2,4)];
M=M/sqrt(det(M));
%[u,s,v]=svd2(M);
%M=u*v'/sqrt(det(u*v'));
end

