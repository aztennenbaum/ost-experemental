function [ output_args ] = calc_q1( b1,b2,r1,r2 )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
output_args=[cross(b1-r1,b2-r2) dot(b1+r1,b2-r2)];
end

