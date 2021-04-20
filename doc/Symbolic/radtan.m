%Basic radtan distortion model
%from https://github.com/PX4/uvc_ros_driver/wiki/Distortion-Models
%negative k1-4 distrotion coefficients ~= inverse

syms x_undist y_undist fcx fcy pcx pcy x y kc1 kc2 kc3 kc4
assume([x_undist y_undist fcx fcy pcx pcy x y kc1 kc2 kc3 kc4],'real')
assume(fcx>0)
assume(fcy>0)


p1 = [(x_undist-pcx)/fcx;(y_undist-pcy)/fcy];
p2 = p1*(p1');
p3 = trace(p2);
p4 = 1+kc1*p3+kc2*p3^2;
p5 = p1*p4 + (2*p2+eye(2)*p3)*[kc4;kc3];
%p6 = p5.*[fcx;fcy]+[pcx;pcy] rescale to image size

J = jacobian(p5,[ x_undist y_undist fcx fcy pcx pcy kc1 kc2 kc3 kc4])

syms n1 n2 n3 n4 n5 n6 n6 n7 n8 n9 n10
p1 = [(x_undist-pcx)/fcx,(y_undist-pcy)/fcy];
p2 = p1'*p1;
p3 = trace(p2);
c0 = [p1,p2(:)',p3, p4, kc1,kc2, kc4]
p4 = (1+kc1*p3+kc2*p3^2);
c0 = [p1,p2(:)',p3, p4, kc3, kc4]

%c1(n1,n2,n3,n4,n5,n6) = p1'*p4 + (2*p2+eye(2)*p3)*[kc4;kc3];
c1(n1,n2,n3,n4,n5,n6,n6,n7,n8,n9,n10) = [n1;n2]*n8 + (2*[n3 n5;n4 n6]+eye(2)*n7)*[n10;n9];