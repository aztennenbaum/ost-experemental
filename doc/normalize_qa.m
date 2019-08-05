function [ out ] = normalize_qa( in )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
	out=sign(in(4))*in(1:4)/sqrt(in(5));
end

