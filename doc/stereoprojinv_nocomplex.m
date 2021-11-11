function [ s ] = stereoprojinv_nocomplex( sp, f, zsign)
% syms x1 y1 z1;
% assume([x1 y1 z1],'real');
% assumeAlso(x1^2+y1^2+z1^2==1)
% simplify(2/sum([x1/(z1+1) y1/(z1+1) 1].^2))
% z1 + 1
[spx,spy]=csplit(sp);
c=2./(spx.^2+spy.^2+1);
c=f.*c./(c-1);
s=cjoin(spx.*c,zsign.*spy.*c);

end

