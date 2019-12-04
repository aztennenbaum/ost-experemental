function [ X ] = sign_nz( X )
%UNTITLED9 Summary of this function goes here
%   Detailed explanation goes here

X=sign(X)+1-abs(sign(X));
%X=sign(X);
end

