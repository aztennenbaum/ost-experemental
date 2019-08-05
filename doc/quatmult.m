function p = quatmult(q, r)
q=q';
r=r';
phi = [q(4)*eye(3) - skew(q(1:3)) ; -q(1:3)'];

p = transpose([phi q]*r);
