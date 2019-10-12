function [ M ] = mobiusmadness( b, r )
% exact solution:
% 
% syms x1 x2 x3 x y1 y2 y3 y
% o(x,y)=[x 1 -x*y -y]'*[x 1 -x*y -y]
% M = null(o(x1,y1)+o(x2,y2)+o(x3,y3));
%  
% 
% M=[x1*y1*y2 - x1*y1*y3 - x2*y1*y2 + x2*y2*y3 + x3*y1*y3 - x3*y2*y3;...
%    x1*x2*y1*y3 - x1*x3*y1*y2 - x1*x2*y2*y3 + x2*x3*y1*y2 + x1*x3*y2*y3 - x2*x3*y1*y3;...
%    x1*y2 - x2*y1 - x1*y3 + x3*y1 + x2*y3 - x3*y2;...
%    x1*x2*y1 - x1*x2*y2 - x1*x3*y1 + x1*x3*y3 + x2*x3*y2 - x2*x3*y3];
% M=[M(1) M(2);M(3) M(4)];
% simplify(det(M))
% (x1 - x2)*(x1 - x3)*(x2 - x3)*(y1 - y2)*(y1 - y3)*(y2 - y3)

 
M=[r(1)*b(1)*b(2) - r(1)*b(1)*b(3) - r(2)*b(1)*b(2) + r(2)*b(2)*b(3) + r(3)*b(1)*b(3) - r(3)*b(2)*b(3);...
   r(1)*r(2)*b(1)*b(3) - r(1)*r(3)*b(1)*b(2) - r(1)*r(2)*b(2)*b(3) + r(2)*r(3)*b(1)*b(2) + r(1)*r(3)*b(2)*b(3) - r(2)*r(3)*b(1)*b(3);...
   r(1)*b(2) - r(2)*b(1) - r(1)*b(3) + r(3)*b(1) + r(2)*b(3) - r(3)*b(2);...
   r(1)*r(2)*b(1) - r(1)*r(2)*b(2) - r(1)*r(3)*b(1) + r(1)*r(3)*b(3) + r(2)*r(3)*b(2) - r(2)*r(3)*b(3)];
 
end