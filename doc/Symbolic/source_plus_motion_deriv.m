syms  Px Py x y x1 x2 y1 y2 totalval maxval psf_radius saturation_radius 


% The following is a piecewise function which maps pixel brightness
% f(Px,Py) for a point source moving from (x1,y1) -> (x2,y2) over a single
% exposure. Since they are linear functions, we can find an analytical
% expression for convolving these with the gausian psf
% 
% Note: we use the computer vision convention, where (0, 0) is the upper-
% left corner of the image
% 
%
%      (x2-0.5,y2-0.5)    +----------------------------+ (x2+0.5,y2-0.5)
%                        /:                           /|
%                       / :                          / |
%                      /  :                         /  |
%                     /   :        0.5+D           /   |
%                    /    :                       /    |
%                   /     :                      /     |
%                  / C+D  :                     /      |
% (x1-0.5,y1-0.5) +-------.--------------------+       |
%                 |       :                    |       |
%                 |       :                    | 0.5+B |
%                 |       :                    |       |
%                 | 0.5+C :         1          |       |
%                 |       :                    |       |
%                 |       :                    |       |
%                 |       + .. .. .. .. .. ..  . .. .. + (x2+0.5,y2+0.5)
%                 |      /                     |  A+B /
%                 |     /                      |     /
%                 |    /                       |    /
%                 |   /          0.5+A         |   /
%                 |  /                         |  /
%                 | /                          | /
%                 |/                           |/
% (x1-0.5,y1+0.5) +----------------------------+ (x1+0.5,y1+0.5)
%
% A(Py)=0.5+(Py-(y2+0.5))/(y2-y1)
% B(Px)=0.5+((x1+0.5)-Px)/(x2-x1)
% C(Px)=0.5+(Px-(x2-0.5))/(x2-x1)
% D(Py)=0.5+((y1-0.5)-Py)/(y2-y1)
%
% convolve w/ PSF and sum the following:
%
% left paralellagram = C
% right paralellagram = B
% top paralellagram = D
% bottom paralellagram = A
% bottom left square (1) = 0.5
% top right square (2) = 0.5 



assume([Px Py x,y],'real')
assume([totalval,maxval,psf_radius,saturation_radius],'positive')
g(Px, Py, x, y, totalval, psf_radius)=totalval*mvnorm([Px-x;Py-y],eye(2)*psf_radius^2);
f(Px, Py, x, y, totalval, psf_radius)=simplify(int(int(totalval*mvnorm([Px-x;Py-y],eye(2)*psf_radius^2),Px,Px-.5,Px+.5),Py,Py-.5,Py+.5));
df(Px, Py, x, y, totalval, psf_radius)=simplify(jacobian(f,[x, y, totalval, psf_radius]));
[val,H]=pixelval_and_jacobian([.1,.2,10,.75],1,2);

test1=double(f(1,2,.1,.2,10,.75))-val
test2=double(df(1,2,.1,.2,10,.75))-H
test3=val-pixelval([.1,.2,10,.75],1,2)

assert(sum(abs(test1))<sqrt(eps))
assert(sum(abs(test2))<sqrt(eps))
assert(sum(abs(test3))<sqrt(eps))

sr(totalval,psf_radius)=combine(solve(g(saturation_radius,0,0,0, totalval, psf_radius)==maxval,saturation_radius),'log')
worst_case_psf_radius=solve(diff(sr,psf_radius)==0,psf_radius)

%Simplification via chainrule

syms n1 n2 n3 n4 n5 n6
c0=[Px-x+.5,Px-x-.5,Py-y+.5,Py-y-.5,1/(sqrt(2)*psf_radius),totalval/4]
c1(n1,n2,n3,n4,n5,n6)=[erf(n1*n5),erf(n2*n5),erf(n3*n5),erf(n4*n5),n5,n6]
c2(n1,n2,n3,n4,n5,n6)=(n1-n2)*(n3-n4)*n6

Jc0=jacobian(c0,[x,y,totalval,psf_radius])
Jc1(n1,n2,n3,n4,n5,n6)=jacobian(c1,[n1,n2,n3,n4,n5,n6])
Jc2(n1,n2,n3,n4,n5,n6)=jacobian(c2,[n1,n2,n3,n4,n5,n6])

c1b  = c1(c0(1),c0(2),c0(3),c0(4),c0(5),c0(6))
c2b  = c2(c1b(1),c1b(2),c1b(3),c1b(4),c1b(5),c1b(6))

Jc1b = Jc1(c0(1),c0(2),c0(3),c0(4),c0(5),c0(6))*Jc0
Jc2b = Jc2(c1b(1),c1b(2),c1b(3),c1b(4),c1b(5),c1b(6))*Jc1b

simplify(c2b -f)
simplify(Jc2b-jacobian(f,[x, y, totalval, psf_radius]))
