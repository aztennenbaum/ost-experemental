function [ out ] = axistheta2a( in )
%UNTITLED11 Summary of this function goes here
%   Detailed explanation goes here
out=[1 0 0;0 cos(in(4)) sin(in(4));0 -sin(in(4)) cos(in(4))]*rotvec(in(1:3))

end

