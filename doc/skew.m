function [ output_args ] = skew( in )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
output_args=[0     -in(3)   in(2) ;...
	         in(3)     0   -in(1) ;...
			-in(2)  in(1)      0  ];

end

