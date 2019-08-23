function [ out ] = axistheta( b1,b2 )
%UNTITLED12 Summary of this function goes here
%   Detailed explanation goes here
vec=rotvec(b1,[0 0 1])*b2';
out=[b1 atan2(-vec(1),vec(2)) norm(vec(1:2))/vec(3)];
end

