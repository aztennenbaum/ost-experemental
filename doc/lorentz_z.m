function [ M ] = lorentz_z( v )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
%alpha=sqrt(sqrt((1-v)/(1+v)));
%M=[alpha 0;0 1/alpha];
%M=normalize_m(division2mobius(sqrt((1+v)/(1-v))));

M=normalize_m([sqrt(1-v) 0;0 sqrt(1+v)]);

end

