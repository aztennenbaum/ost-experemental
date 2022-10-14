function [ out ] = Rx( roll )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

out=[1 0 0;0 cos(roll) -sin(roll); 0 sin(roll) cos(roll)];
end

