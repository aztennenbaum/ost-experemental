function [ M ] = q2mobius( q)
% see visual complex analysis p.289 (note there is a mistake)
 a= q(4) - q(3)*1i;
 b=-q(2) + q(1)*1i;
 M=[a b;-conj(b) conj(a)];

%M=[q(4)-q(3)*1i -q(2)+q(1)*1i;q(2)+q(1)*1i q(4)+aq(3)*1i];
end
