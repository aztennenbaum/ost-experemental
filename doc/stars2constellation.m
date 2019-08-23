function [ c ] = stars2constellation( s )
%UNTITLED13 Summary of this function goes here
%   Detailed explanation goes herer(:,1)+r(:,2)
s=reshape(s(:)',[3,numel(s)/3]);
axis=axistheta(normalize(s(:,1)+s(:,2))',s(:,2)');
c=axistheta2a(axis)*s(:,3:end)
c=c(1:2,:)./(c(3,:)*axis(end));
c=[axis c(:)'];
end

