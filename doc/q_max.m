function [ out ] = q_max( q1, q2 )
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here
if q1(5)>q2(5)
	out=q1;
else
	out=q2;
end
end

