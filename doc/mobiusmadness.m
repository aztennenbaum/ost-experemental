function [ M ] = mobiusmadness( r, b )
%mobius(M,b)=r
% exact solution:
% 
% syms x1 x2 x3 x y1 y2 y3 y
% o(x,y)=[x 1 -x*y -y]'*[x 1 -x*y -y]
% M = null(o(x1,y1)+o(x2,y2)+o(x3,y3));
% %  
% % %multiply by divisor
% M=M*(x1*x2*y1 - x1*x2*y2 - x1*x3*y1 + x1*x3*y3 + x2*x3*y2 - x2*x3*y3);

% M=[x1*y1*y2 - x1*y1*y3 - x2*y1*y2 + x2*y2*y3 + x3*y1*y3 - x3*y2*y3;...
%    x1*x2*y1*y3 - x1*x3*y1*y2 - x1*x2*y2*y3 + x2*x3*y1*y2 + x1*x3*y2*y3 - x2*x3*y1*y3;...
%    x1*y2 - x2*y1 - x1*y3 + x3*y1 + x2*y3 - x3*y2;...
%    x1*x2*y1 - x1*x2*y2 - x1*x3*y1 + x1*x3*y3 + x2*x3*y2 - x2*x3*y3];
% 
% M2=[[y1 y2 y3]*skew([1 1 1])*([x1;x2;x3].*[y1;y2;y3]);
%    -[y1 y2 y3]*skew([x1 x2 x3])*([x1;x2;x3].*[y1;y2;y3]);...
%     [y1 y2 y3]*skew([1 1 1])*[x1;x2;x3];
% 	[x1 x2 x3]*skew([1 1 1])*([x1;x2;x3].*[y1;y2;y3])];
% 
% M2=[cross([1 1 1],[x1 x2 x3].*[y1 y2 y3])*[y1;y2;y3];
%     cross([x1 x2 x3],[y1 y2 y3])*([x1;x2;x3].*[y1;y2;y3]);...
%     cross([x1 x2 x3],[y1 y2 y3])*[1;1;1];
% 	cross([1 1 1],[x1 x2 x3].*[y1 y2 y3])*[x1;x2;x3]];
% % 
% % M=[M(1) M(2);M(3) M(4)];
% % simplify(det(M))
% % (x1 - x2)*(x1 - x3)*(x2 - x3)*(y1 - y2)*(y1 - y3)*(y2 - y3)
% 
% 
M=[cross([1 1 1],b.*r)*transpose(r) cross(b,r)*transpose(b.*r);...
    cross(b,r)*[1;1;1] cross([1 1 1],b.*r)*transpose(b)];

% M=M/sqrt(det(M));
end