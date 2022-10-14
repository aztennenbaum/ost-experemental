function [ out ] = Ry( pitch )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

out=[cos(pitch) 0 sin(pitch);0 1 0;-sin(pitch) 0 cos(pitch)];
end

