q=randn([4 1]);
q=q/norm(q);
A=q2a(q)
%A=eye(3);
r1=randn([3 1]);
r1=r1/norm(r1);
r2=randn([3 1]);
r2=r2/norm(r2);
r3=randn([3 1]);
r3=r3/norm(r3);
b1=A*r1;
b2=A*r2;
b3=A*r3;


b=[b1 b2 b3];
r=[r1 r2 r3];

Q4=calc_q3([b1(1) -b1(2) -b1(3)],...
        [b2(1) -b2(2) -b2(3)],...
        [r1(1) r1(2) r1(3)],...
        [r2(1) r2(2) r2(3)]);
Q4=[Q4(4) Q4(3) -Q4(2) -Q4(1)];

n4=norm(Q4);
Q4=sign(Q4(4))*Q4/norm(Q4);

Q3=calc_q3([-b1(1) b1(2) -b1(3)],...
        [-b2(1) b2(2) -b2(3)],...
        [r1(1) r1(2) r1(3)],...
        [r2(1) r2(2) r2(3)]);
Q3=[ -Q3(3)  Q3(4)  Q3(1) -Q3(2)];

n3=norm(Q3);
Q3=sign(Q3(4))*Q3/norm(Q3);

Q2=calc_q3([-b1(1) -b1(2) b1(3)],...
        [-b2(1) -b2(2) b2(3)],...
        [r1(1) r1(2) r1(3)],...
        [r2(1) r2(2) r2(3)])
Q2=[Q2(2) -Q2(1)  Q2(4) -Q2(3)];

n2=norm(Q2);
Q2=sign(Q2(4))*Q2/norm(Q2);
	
Q1=calc_q3([b1(1) b1(2) b1(3)],...
           [b2(1) b2(2) b2(3)],...
           [r1(1) r1(2) r1(3)],...
           [r2(1) r2(2) r2(3)]);
n1=norm(Q1);
Q1=sign(Q1(4))*Q1/norm(Q1);

% QQ=normalize_q([quatmult([1 0 0 0],calc_q32(q2a([1 0 0 0])*b,r));...
%                 quatmult([0 1 0 0],calc_q32(q2a([0 1 0 0])*b,r));...
%                 quatmult([0 0 1 0],calc_q32(q2a([0 0 1 0])*b,r));...
%                 quatmult([0 0 0 1],calc_q32(q2a([0 0 0 1])*b,r))])
Q = q_seq_rot(b,r);

q2a(normalize_qa(q_append_norm_sq(calc_q1([1 0 0],[0 1 0],-(r1+r2)'/norm(r1+r2),(r1-r2)'/norm(r1-r2)))))*q1r
