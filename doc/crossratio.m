function [ out ] = crossratio( a,b,c,d )

out=((a-c).*(b-d))./((b-c).*(a-d));
end

