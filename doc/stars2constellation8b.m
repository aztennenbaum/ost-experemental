function [ c ] = stars2constellation8b( s,zsign )
%UNTITLED13 Summary of this function goes here
%   Detailed explanation goes herer(:,1)+r(:,2)
c=stereoproj([normalize(s(:,1))+normalize(s(:,2)) s(:,2:end)],zsign);
c(2)=mobius(s2mobius(-c(1)),c(2));
c(3:end)=mobius(division2mobius(c(2))*s2mobius(-c(1)),c(3:end));
end
