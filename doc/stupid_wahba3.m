function [ M ] = stupid_wahba3( x, y )
%UNTITLED8 Summary of this function goes here
%   Detailed explanation goes here
% [u,s,v]=svd([x-y (x+y)*1i 1+x.*y (1-x.*y)*1i])
A=-[x ones(size(x)) -x.*y -y]'*[x ones(size(x)) -x.*y -y];
[v,s]=eig(A);

M=[v(1,4) v(2,4);v(3,4) v(4,4)];
M=M/sqrt(det(M));
[u,s,v]=svd2(M);
M=u*v'/sqrt(det(u*v'));
end