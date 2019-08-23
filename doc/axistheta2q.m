function [ out ] = axistheta2q( in )
%UNTITLED11 Summary of this function goes here
%   Detailed explanation goes here
out=normalize(quatmult([[0 0 1]*-sin(in(4)/2) -cos(in(4)/2)],[cross([0 0 1],in(1:3)),dot([0 0 1],in(1:3))+1]))
end

