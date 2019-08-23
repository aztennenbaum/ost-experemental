function [ out ] = a2axistheta( in )
%UNTITLED10 Summary of this function goes here
%   Detailed explanation goes here
%b1=rotvec2(in(3,:),in(2,:),[0 0 1]);
b1=rotvec(in(3,:),[0 0 1])*in(2,:)';
out=[in(3,:) atan2(-b1(1),b1(2))];
end

