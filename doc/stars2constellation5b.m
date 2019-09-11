function [ c ] = stars2constellation5b( s )
%UNTITLED13 Summary of this function goes here
%   Detailed explanation goes herer(:,1)+r(:,2)
s(:,1)=normalize(s(:,1)+s(:,2));
s(:,2:end)=q2a(normalize(rotvec2(transpose(s(:,1)))))*s(:,2:end);
c=([1 1i 0]*s)./(1-s(3,:));
c(3:end)=c(3:end)./c(2:end-1);
end

% simplify(normalize(simplify(quatmult(rotvec2(b),qinv(rotvec(b))))))
%  
% ans =
%  
% [ 0, 0, x1/(1 - z1^2)^(1/2), -y1/(1 - z1^2)^(1/2)]
%  
% simplify(normalize(simplify(quatmult(rotvec(b),qinv(rotvec2(b))))))
%  
% ans =
%  
% [ 0, 0, -x1/(1 - z1^2)^(1/2), -y1/(1 - z1^2)^(1/2)]
% expand((y1-i*x1)^2)
%  
% ans =
%  
% - x1^2 - x1*y1*2i + y1^2