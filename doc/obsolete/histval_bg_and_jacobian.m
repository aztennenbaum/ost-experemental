function [ val, H ] = histval_bg_and_jacobian( params, Pval, step_sz ) 

  m = params(1);
  s = params(2);
  f1 = (Pval - m)/sqrt(2*m);
  f2 = (Pval - m + step_sz)/sqrt(2*m);
  fv = erf(f2) - erf(f1);

  c1 = erf(sqrt(m/2)) + 1;
  c2 = sqrt(2*m*pi);
  r1 = exp(-f1.^2);
  r2 = exp(-f2.^2);

    
  val=s*fv/c1;
  H=[ -(s*exp(-m/2)*fv)/(c2*c1^2) - (r2.*(Pval + m + step_sz) - r1.*(Pval + m))*s/(c1*c2*m), fv/c1];
end
