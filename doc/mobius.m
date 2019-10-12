function [ z ] = mobius( M,z )
%see visual complex analysis p.289
z=(M(1,1)*z+M(1,2))./(M(2,1)*z+M(2,2));
end

