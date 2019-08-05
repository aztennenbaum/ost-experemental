function [ out ] = rotvec( in )
%UNTITLED9 Summary of this function goes here
%   Detailed explanation goes here
b1=skew([0;in(3);-in(2)]);
out=eye(3)+b1*(eye(3) + b1/(in(1) + 1));
end
