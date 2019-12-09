function [ c, v ] = stereoproj2( s )
%UNTITLED Summary of this function goes here
% stereoproj(s,-1)=-1./stereoproj(s,-1)
zsign=sign_nz(s(3,:));
v=zsign.*sqrt(sum(s.^2,1));
c=(s(1,:)+1i*zsign.*s(2,:))./(s(3,:)+v);
end

