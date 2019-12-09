 function [ vqs ] = mobius2vqs( M )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
[u,s,v]=svd2b(M);
q = real(mobius2q(u*v'))';
v = lorentz_z_inv(s);
s = u(1,2)/u(2,2);
vqs = [v q real(s) imag(s)];
end

