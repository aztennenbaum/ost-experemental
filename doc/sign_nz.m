function [ X ] = sign_nz( X )
%if X=0 return 1
%else return sign(X)

X=sign(X)+1-abs(sign(X));
%X=sign(X);
end

