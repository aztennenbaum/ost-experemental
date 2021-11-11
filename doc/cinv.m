function [ out ] = cinv( in )
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here

[in_real,in_imag]=csplit(in);
in_abs_sq=(in_real.^2+in_imag.^2);
out=cjoin(in_real./in_abs_sq,-in_imag./in_abs_sq);
end

