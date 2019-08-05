function [ output_args ] = calc_q32( b,r )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
output_args=transpose([cross(b(:,1)-r(:,1),b(:,2)-r(:,2));dot(b(:,2),r(:,1))-dot(b(:,1),r(:,2))]);

end

