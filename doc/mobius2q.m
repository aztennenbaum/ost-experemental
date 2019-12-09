function [ q ] = mobius2q( M)
% Derived from q2mobius
M=normalize_m(M);
q=[-M(1,2)*1i-M(2,1)*1i;M(2,1)-M(1,2);M(1,1)*1i-M(2,2)*1i;M(1,1)+M(2,2)]/2;
end
