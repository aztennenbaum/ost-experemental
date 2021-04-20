function [ out ] = mvnorm( x,v )

k=numel(x);
out=exp(-(x'*inv(v)*x)/2)/sqrt((2*sym(pi))^k*det(v));
end

