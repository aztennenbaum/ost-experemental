function invq = qinv2(q)

invq = [-q(1:3);q(4)]/sum(q.^2);
