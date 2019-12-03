function [ U, S, V ] = svd2b( X )
%Closed form svd for 2x2 complex matrices
    X=X/sqrt(det(X));
    P = X*X';
    a=P(1,1);
    b=P(1,2);
    c=P(2,1);
    d=P(2,2);
    
    sgn1=abs(a-d)/(a-d);%abs(a-d) = 0 -> pure rotation
    
    root = sgn1*((a+d)^2-4)^(1/2);
    s1 = (a + d + root)/2;
    s2 = (a + d - root)/2;
    U = sgn1*[a-s2 -b;c s1-d];
    U = U/sqrt(det(U));
    S = [sqrt(s1) 0;0 sqrt(s2)];
    V = (inv(S)*U'*X)';
    
    sgn2=abs(trace(V))/trace(V);
    
    S = sgn2*S;
    V = sgn2*V;
    mobius2q(U)
    mobius2q(V)
end

