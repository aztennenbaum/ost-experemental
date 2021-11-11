function [ out_real, out_imag ] = csplit( in )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here

out_real=in(:,1:2:end);
out_imag=in(:,2:2:end);
end

