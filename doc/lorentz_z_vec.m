function [ vec ] = lorentz_z_vec( v, vec)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

l=1/sqrt(1-v^2);
vec(3)=l*(vec(3)+v*vec(4));
vec(4)=l*(vec(4)+v*vec(3));
end

