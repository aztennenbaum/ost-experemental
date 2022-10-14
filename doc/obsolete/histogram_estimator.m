%poisson(mean_photons)/p ~ N(mean_photons/p,mean_photons/p^2)
%v=m/p
syms m p Pval totalval step_sz
assume([m p Pval totalval step_sz],'positive')
g(Pval, m, totalval, p)=totalval*mvnorm((Pval-m)/p,m/p^2);
f(Pval, m, totalval, p, step_sz)=simplify(int(g,Pval,Pval,Pval+step_sz));
df(Pval, m, totalval, p, step_sz)=simplify(jacobian(f,[m, p]));
[val,H]=histval_and_jacobian([.2,.75],2,10,.69);

test1=double(f(2,.2,10,.75,.69))-val
test2=double(df(2,.2,10,.75,.69))-H

assert(sum(abs(test1))<sqrt(eps))
assert(sum(abs(test2))<sqrt(eps))

t2s=p*(erf(sqrt(m/2)) + 1)/2;
s=totalval*t2s;
simplify(s-simplify(int(g(Pval, m, totalval, p),Pval,0,inf)))
