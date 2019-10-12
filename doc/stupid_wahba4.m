function [ M ] = stupid_wahba4( x, y )
%UNTITLED8 Summary of this function goes here
%   Detailed explanation goes here

A=[x ones(size(x)) -x.*y -y]'*[x ones(size(x)) -x.*y -y];

%first rayleigh quotient iteration to find smallest eigenvector
w=(A\[1;0;0;1]);
w=(A\w);

%uncomment if additional iterations are needed
%w=normalize(w);w=(A-(w'*A*w)*eye(4))\w;

M=[w(1) w(2);w(3) w(4)]
M=M/sqrt(det(M))
[u,s,v]=svd2(M);
M=u*v'/sqrt(det(u*v'));
end
