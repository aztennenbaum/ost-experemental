function [ out ] = mvnorm( x,v )

k=numel(x);
out=2*sym(pi)^(-k/2)*det(v)^-(1/2)*exp(-(x'*inv(v)*x)/2);
end

