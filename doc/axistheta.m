function [ out ] = axistheta( b1,b2 )
%UNTITLED12 Summary of this function goes here
%   Detailed explanation goes here

vec=rotvec2(b1,b2);
out=[transpose(b1) atan2(vec(3),vec(2))];
end

