function [ out ] = normalize( in )
%UNTITLED15 Summary of this function goes here
%   Detailed explanation goes here
out=in/sqrt(sum(in.^2));
end

