function [ c ] = stars2constellation6b( s )
%UNTITLED13 Summary of this function goes here
%   Detailed explanation goes herer(:,1)+r(:,2)
s(:,1)=normalize(s(:,1)+s(:,2));
vec=[0;0;-1];
s=s+vec;

c=[1 i 0 0;0 0 1 i]*[[s(:,1);0] quatmult2([cross(vec,s(:,1));dot(vec,s(:,1))],[s(:,2:end);zeros([1 numel(s)/3-1])])];
c=c(1,:)./conj(c(2,:));
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