function [ q ] = q_seq_rot( b, r )
q=normalize_q([quatmult([1 0 0 0],calc_q32(q2a([1 0 0 0])*b,r));...
               quatmult([0 1 0 0],calc_q32(q2a([0 1 0 0])*b,r));...
               quatmult([0 0 1 0],calc_q32(q2a([0 0 1 0])*b,r));...
               quatmult([0 0 0 1],calc_q32(q2a([0 0 0 1])*b,r))]);
end

