function [ out ] = sqrt_q( in )
%UNTITLED8 Summary of this function goes here
%   Detailed explanation goes here
s=sqrt(2*(in(4)+sign(in(4))))
out=[in(1:3)/s s/2] 
end

