function [ s ] = constellation2stars( c )
%UNTITLED17 Summary of this function goes here
%   Detailed explanation goes here
axis=c(1:6);
c=reshape(c(6:end),[2,(numel(c)-5)/2]);
s=[axis(5)*[0 0 c(1,:);-1 1 c(2,:)];ones([1,2+size(c,2)])];
s=axistheta2a(axis)'*s./sqrt(sum(s.^2,1));
end

