% This function transforms an attitude matrix into quaternions.
% Andrew Dianetti
% 3 December 2014

function q = a2q(A)
    q4=0.5*sqrt(1+trace(A));
    q1=1/(4*q4)*(A(2,3)-A(3,2));
    q2=1/(4*q4)*(A(3,1)-A(1,3));
    q3=1/(4*q4)*(A(1,2)-A(2,1));
    q=[q1;q2;q3;q4];
end