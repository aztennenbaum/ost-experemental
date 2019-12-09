function [ v ] = lorentz_z_inv( M )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here

alpha=(M(1,1)/M(2,2))^2;
v=-(alpha - 1)/(alpha + 1);
end

