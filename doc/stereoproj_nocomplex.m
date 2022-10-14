function [ sp ] = stereoproj_nocomplex( s, f, zsign)
%UNTITLED Summary of this function goes here
% stereoproj(s,-1)=-1./stereoproj(s,1)
[sx,sy]=csplit(s);
v=zsign.*sqrt(sx.^2+sy.^2+f.^2);
sp=cjoin(sx./(f+v),zsign.*sy./(f+v));
end

