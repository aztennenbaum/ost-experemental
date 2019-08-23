function [ out ] = rotvec( in,vec )
%UNTITLED9 Summary of this function goes here
%   Detailed explanation goes here
%b1=skew(cross(in,vec));
%out=eye(3)+b1*(eye(3) + b1/(dot(in,vec) + 1));
out=q2a(normalize([cross(vec,in),dot(vec,in)+1]));
end
