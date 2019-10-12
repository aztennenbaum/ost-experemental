function [ v ] = lorentz_z_inv( M )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here

v=-(M(1,1)^4 - 1)/(M(2,2)^4 + 1);

end

