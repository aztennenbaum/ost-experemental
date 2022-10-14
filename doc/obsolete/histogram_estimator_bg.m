%poisson(mean_photons)/p ~ N(mean_photons/p,mean_photons/p^2)
%v=m/p
syms m p Pval s step_sz

t2s=p*(erf(sqrt(m/2)) + 1)/2;
totalval=s/t2s;
assume([m p Pval s step_sz],'positive')
g(Pval, m, s, p)=totalval*mvnorm((Pval-m)/p,m/p^2);
f(Pval, m, s, p, step_sz)=simplify(int(g,Pval,Pval,Pval+step_sz));
df(Pval, m, s, p, step_sz)=simplify(jacobian(f,[m,s]));
[val,H]=histval_bg_and_jacobian([.2,10],2,.69);

test1=double(f(2,.2,10,.75,.69))-val
test2=double(df(2,.2,10,.75,.69))-H

assert(sum(abs(test1))<sqrt(eps))
assert(sum(abs(test2))<sqrt(eps))

simplify(s-simplify(int(g(Pval, m, s, p),Pval,0,inf)))
