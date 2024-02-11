function [ out ] = crossratio_jacobian( a,b,c,d )
%derivation
%syms a b c d
%simplify(jacobian(crossratio(a,b,c,d),[a b c d])-crossratio_jacobian(a,b,c,d))

out=[crossratio(a,b,c,d).*(1./(a-c)-1./(a-d)),...
     crossratio(a,b,c,d).*(1./(b-d)-1./(b-c)),...
     crossratio(a,b,c,d).*(1./(b-c)-1./(a-c)),...
     crossratio(a,b,c,d).*(1./(a-d)-1./(b-d))];
end

