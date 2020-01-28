function [ mrp ] = q2mrp( q)
% Convert unnormalized quaternion to modified rodrigues parameter
mrp=q(1:3)/(q(4)+sign_nz(q(4))*norm(q));
end
