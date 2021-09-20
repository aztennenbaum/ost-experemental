function [ val, H ] = histval_and_jacobian( params, Pval, totalval, step_sz ) 

m = params(1);
p = params(2);
f1 = (Pval - m)/sqrt(2*m);
f2 = (Pval - m + step_sz)/sqrt(2*m);
fv = (erf(f2) - erf(f1))/2;
r1 = exp(-f1.^2);
r2 = exp(-f2.^2);
r3 = (r1.*(Pval + m)-r2.*(Pval + m + step_sz))/sqrt(8*pi*m^3);

val=p*totalval*fv; 
H=[p*totalval*r3,totalval*fv];

end
