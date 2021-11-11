function [ sp ] = stereoproj2( s, f )
%UNTITLED Summary of this function goes here
% stereoproj(s,-1)=-1./stereoproj(s,1)
%zsign=1;
sp=s./(sqrt(s.*conj(s)+f^2)+f);
end

