function [ U, S, V] = svd2( X )
%Closed form svd for 2x2 complex matrices
    X=X/sqrt(det(X));
    P = X*X';
    
    s_order=sign_nz(P(1,1)-P(2,2));
    tr=trace(P);
    
    root = s_order*(tr^2-4)^(1/2);
    Sp_sq = [tr-root 0;0 tr+root]/2;
    
    U = s_order*(P-Sp_sq)*[1 0;0 -1];
    U = U/sqrt(det(U));
    
    
    Sp = Sp_sq.^(1/2);
    S  = inv(Sp);
    V  = (Sp*U'*X)';
    
%     uq = mobius2q(U);
%     E = [(norm(mobius2q(X-U*S*V')));...
%          (norm(imag(mobius2q(U))));...
%           norm(uq(3));...
%          (norm(imag(mobius2q(V))))];
%      ecount = 0 ; for a=[1:100000]; [u s v e] = svd2(randn(2,2)+1i*randn(2,2)); if any(e>eps*10) ecount=ecount+1; end; end; ecount

end

