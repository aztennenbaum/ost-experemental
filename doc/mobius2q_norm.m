function [ q ] = mobius2q_norm( M )
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here
%M=M/sqrt(det(M));
q=[imag(M(1,2));...
   real(M(2,1));...
   imag(M(2,2));...
   real(M(1,1))];
q=q/(sign_nz(q(4))*norm(q));
end
