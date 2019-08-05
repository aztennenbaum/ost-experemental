function [ out ] = rotvec2( in1, in2 )
%UNTITLED9 Summary of this function goes here
%   Detailed explanation goes here
b1=[0;in1(3);-in1(2)];
out=in2+cross(b1,in2+cross(b1,in2/(in1(1)+1)));
end