function [ out ] = a2axistheta( in )
%UNTITLED10 Summary of this function goes here
%   Detailed explanation goes here
b1=rotvec2(in(1,:),in(2,:));
out=[in(1,:) atan2(b1(3),b1(2))];
end

