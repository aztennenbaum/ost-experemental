function [ q ] = mrp2q( mrp)
% Convert modified rodrigues parameter to quaternion
q=[2*mrp;1-sum(mrp.^2)];
end
