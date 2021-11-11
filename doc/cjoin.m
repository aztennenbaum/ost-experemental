function [ out ] = cjoin( in_real,in_imag )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here

 
% interlave two same sized matrices by column
a = in_real';
b = in_imag';
out = reshape([a(:) b(:)]',2*size(a,1), [])';
end

