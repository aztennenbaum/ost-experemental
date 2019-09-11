function [ out ] = rotvec2( in )
%hairy ball theorem says we have to have a discontinuity somewhere, so lets
%put it at z=0

out=quatmult(rotvec(in*q2a([1 0 0 0])),[1 0 0 0]);
end