function [ U, S, V ] = svd2( X )
%Closed form svd for 2x2 complex matrices
    P = X*X';
    a=P(1,1);
    b=P(1,2);
    c=P(2,1);
    d=P(2,2);

    root = (a^2 - 2*a*d + d^2 + 4*b*c)^(1/2);
    s1 = (a + d - root)/2;
    s2 = (a + d + root)/2;
% 
%     va = [d-s1 -b;-c a-s1];
%     vb = [-b d-s2;a-s2 -c];
	
	va = [d-s1 -b;-c a-s1];
    vb = [b d-s2;s2-a -c];
	
 	val=sum(va.*conj(va),1);
    [val,idx]=max(val);
	if abs(val)<100*eps
		U=eye(2);
	else
		U = [va(:,idx) vb(:,idx)]/sqrt(val);
	end
	if (idx==2)
		U = normalize_m(U*[0 -vb(1,2);-va(2,2) 0]);
		S = [sqrt(s2) 0;0 sqrt(s1)];
	else
		S = [sqrt(s1) 0;0 sqrt(s2)];
	end	
	det(U)
    V = (inv(S)*U'*X)';
	det(V)
   
end