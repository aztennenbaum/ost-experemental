function [ M ] = lorentz_z( v )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
alpha=sqrt(sqrt((1-v)/(1+v)));
M=[alpha 0;0 1/alpha];
end

