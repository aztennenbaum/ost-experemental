function p = quatmult2(q, r)

phi = [q(4)*eye(3) - skew(q(1:3)) ; -q(1:3)'];

p = [phi q]*r;
