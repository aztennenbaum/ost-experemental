function [ out ] = cmult( in1,in2 )
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here

[in1_real,in1_imag]=csplit(in1);
[in2_real,in2_imag]=csplit(in2);
out=cjoin(in1_real.*in2_real-in1_imag.*in2_imag,in1_real.*in2_imag+in1_imag.*in2_real);
end

