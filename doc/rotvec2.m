function [ out ] = rotvec2( in1, in2 ,vec)
%UNTITLED9 Summary of this function goes here
%   Detailed explanation goes here
b1=cross(in1,vec);
out=in2+cross(b1,in2+cross(b1,in2/(dot(in1,vec)+1)));
end