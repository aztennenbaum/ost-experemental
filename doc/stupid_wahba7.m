function [ M, vqs ] = stupid_wahba7( x, y )
%UNTITLED8 Summary of this function goes here
%   Detailed explanation goes here
A=[x ones(size(x)) -x.*y -y]'*[x ones(size(x)) -x.*y -y];
A=adjlower4(chol(A+trace(A)*eye(4)*eps,'lower'));
%do a few inverse power iterations to find smallest eigenvector
w=normalize(A*[1;0;0;1]);
w=normalize(A'*w);
w=normalize(A*w);
w=normalize(A'*w);
w=normalize(A*w);
w=normalize(A'*w);

vqs=mobius2vqs([w(1) w(2);w(3) w(4)]);
M=q2mobius(vqs(2:5));
end
