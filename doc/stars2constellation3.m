function [ c ] = stars2constellation3( s )
%UNTITLED13 Summary of this function goes here
%   Detailed explanation goes herer(:,1)+r(:,2)
c_center=transpose(normalize(s(:,1)+s(:,2)));
stars=[1 1i 0]*q2a(normalize(rotvec2(c_center)))*s(:,2:end);
c=[(c_center(1)+1i*c_center(2))/(1-c_center(3)) stars(:,1) stars(:,2:end)/stars(:,1)];
end
%k3*-sign(k3(1))^2
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
% 
% k2=stars2constellation2(b)
% 
% k2 =
% 
%   -0.1889 - 0.4041i   0.3076 + 0.9062i  -0.4970 + 0.0568i
%   
% k3=stars2constellation3(b)
% 
% k3 =
% 
%   -0.9492 - 2.0310i  -0.4979 + 0.8173i  -0.4970 + 0.0568i
% 
% k3(2)*-sign(k3(1))^2
% 
% ans =
% 
%    0.3076 + 0.9062i
%   