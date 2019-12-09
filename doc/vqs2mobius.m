function [ M ] = vqs2mobius( vqs )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
v=vqs(1);
q=vqs(2:5);%body frame to reference frame quaternion
s=0;
if numel(vqs)==7
	s=vqs(6)+1j*vqs(7);
end
M=normalize_m(s2mobius(s)*lorentz_z( v )*s2mobius(-s)*q2mobius(q));
end

