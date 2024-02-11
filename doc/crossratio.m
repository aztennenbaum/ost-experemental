function [ z ] = crossratio( a,b,c,d )
%calculate cross ratio
%z
%

z=((a-c).*(b-d))./((b-c).*(a-d));
end

