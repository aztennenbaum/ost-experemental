function [ s ] = stereoproj2inv( c, zsign )
% syms x1 y1 z1;
% assume([x1 y1 z1],'real');
% assumeAlso(x1^2+y1^2+z1^2==1)
% simplify(2/sum([x1/(z1+1) y1/(z1+1) 1].^2))
% z1 + 1
s=[zsign*(c+conj(c))/2;(c-conj(c))/2i;zsign*ones(size(c))];
s=s*2./sum(s.^2,1)-[0;0;1];
end

