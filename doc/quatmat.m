function [ phi ] = quatmat( q )
%UNTITLED13 Summary of this function goes here
%   Detailed explanation goes herer(:,1)+r(:,2)
phi=[[q(4)*eye(3) - skew(q(1:3)) ; transpose(-q(1:3))] q]
end

% simplify(normalize(simplify(quatmult(rotvec2(b),qinv(rotvec(b))))))
%  
% ans =
%  
% [ 0, 0, x1/(1 - z1^2)^(1/2), -y1/(1 - z1^2)^(1/2)]
%  
% simplify(normalize(simplify(quatmult(rotvec(b),qinv(rotvec2(b))))))
%  
% ans =
%  
% [ 0, 0, -x1/(1 - z1^2)^(1/2), -y1/(1 - z1^2)^(1/2)]
% expand((y1-i*x1)^2)
%  
% ans =
%  
% - x1^2 - x1*y1*2i + y1^2