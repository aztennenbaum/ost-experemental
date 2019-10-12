function [ s ] = constellation2stars8b( c,zsign )
%UNTITLED17 Summary of this function goes here
%   Detailed explanation goes here
% syms x1 y1 z1;
% assume([x1 y1 z1],'real');
% assumeAlso(x1^2+y1^2+z1^2==1)
% simplify(2/sum([x1/(z1+1) y1/(z1+1) 1].^2))
% z1 + 1
s1=[-1 1 c(3:end)]*c(2);
s2=mobius(s2mobius(c(1)),s1);
s=stereoprojinv(s2,zsign);
end

