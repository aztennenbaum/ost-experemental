function [ out ] = axistheta2a( in )
%UNTITLED11 Summary of this function goes here
%   Detailed explanation goes here
%out=[cos(in(4)) sin(in(4)) 0;-sin(in(4)) cos(in(4)) 0;0 0 1]*rotvec(in(1:3),[0 0 1]);
out=[cos(in(4)) sin(in(4)) 0;-sin(in(4)) cos(in(4)) 0;0 0 1]*rotvec(in(1:3),[0 0 1]);

end

