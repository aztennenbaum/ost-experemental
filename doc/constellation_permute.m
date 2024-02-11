function [ out ] = constellation_permute( in, idx )
%UNTITLED5 Summary of this function goes here
% see crossratio_jacobian_score_nocomplex2( in )

a=[1 2];
b=[3 4];
c=[5 6];
d=[7 8];

%crossratio(a,b,c,d)=z
%crossratio(a,c,b,d)=1-z
%crossratio(a,d,b,c)=1-1/z
%crossratio(a,b,d,c)=1/z
%crossratio(a,c,d,b)=1/(1-z)
%crossratio(a,d,c,b)=1-1/(1-z)
permutations=[a,b,c,d;...
              a,c,b,d;...
              a,d,b,c;...
              a,b,d,c;...
              a,c,d,b;...
              a,d,c,b];

col=permutations(sub2ind(size(permutations),repmat(idx,1,8),repmat(1:8,numel(idx),1)));
row=repmat(transpose(1:numel(idx)),1,8);
out=in(sub2ind(size(in),row,col));
end

