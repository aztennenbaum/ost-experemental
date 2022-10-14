
%Matlab code snippet for verifying threshold calculation

syms f1 f2 fn a1 a2 an x
f=[f1 f2 fn];
a=[a1 a2 an];

ss=int(f1,x,a1,a1+1)+int(f2,x,a2,a2+1)+int(fn,x,an,an+1);
mm=(int(f1*x,x,a1,a1+1)+int(f2*x,x,a2,a2+1)+int(fn*x,x,an,an+1))/ss;
vv=(int(f1*(x-mm)^2,x,a1,a1+1)+int(f2*(x-mm)^2,x,a2,a2+1)+int(fn*(x-mm)^2,x,an,an+1))/ss;

s=sum(f);
m=sum(f.*a)/s+1/2;
v=sum(f.*a.*a)/s - (m-1/2)^2 + 1/12;

%returns zero if they are equivalent
simplify(ss-s)
simplify(mm-m)
simplify(vv-v)



