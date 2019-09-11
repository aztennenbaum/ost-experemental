function p = quatmult(q, r)
 p = [q(4)*r(1:3)+r(4)*q(1:3)-cross(q(1:3),r(1:3)) q(4)*r(4)-sum(q(1:3).*r(1:3))];
 