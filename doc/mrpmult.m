function p_out = mrpmult(p1, p2)
    p_out=q2mrp(quatmult(mrp2q(p1),mrp2q(p2)));
end
