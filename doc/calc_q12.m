function [ output_args ] = calc_q12( b,r )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
output_args=transpose([cross(b(:,1)-r(:,1),b(:,2)-r(:,2));dot(b(:,1)+r(:,1),b(:,2)-r(:,2))]);
%=[cross(b1-r1,b2-r2) dot(b1+r1,b2-r2)];
%=[cross(b1-r1,b2-r2) dot(b2,r1)-dot(b1,r2)];
end

