function [ out ] = Rz( yaw )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

out=[cos(yaw) -sin(yaw) 0;sin(yaw) cos(yaw) 0;0 0 1];
end

