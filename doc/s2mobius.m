function [ M ] = s2mobius(s)
%generate the mobius transform that rotates 0 to s
M=normalize_m([1 s;-conj(s) 1]);
end

