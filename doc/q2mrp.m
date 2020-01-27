function [ mrp ] = q2mrp( q)
% Convert quaternion to modified rodrigues parameter
mrp=q(1:3)/(q(4)+1);
end
