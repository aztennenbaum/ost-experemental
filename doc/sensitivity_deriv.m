syms q1 q2 q3 q4 dq1 dq2 dq3 dq4 b1x b1y b1z b2x b2y b2z
assume([q1 q2 q3 q4 dq1 dq2 dq3 dq4 b1x b1y b1z b2x b2y b2z],'real')

b1=[b1x;b1y;b1z];
b2=[b2x;b2y;b2z];
%r1=A*b1
%r2=A*b2

SQ=q2a([q1 q2 q3 q4]);

dSQ1=diff(SQ,q1)
dSQ2=diff(SQ,q2)
dSQ3=diff(SQ,q3)
dSQ4=diff(SQ,q4)

SQx=SQ+dSQ1*dq2+dSQ2*dq2+dSQ3*dq3+dSQ4*dq4

%
%[cross(SQx*b1,SQx*b2);dot(SQx*b1,SQx*b2)]
%
%syms SQr1_dot_b1 dSQ1r1_dot_b1 dSQ2r1_dot_b1 dSQ3r1_dot_b1 dSQ4r1_dot_b1
%syms SQr2_dot_b1 dSQ1r2_dot_b1 dSQ2r2_dot_b1 dSQ3r2_dot_b1 dSQ4r2_dot_b1
%syms SQr3_dot_b1 dSQ1r3_dot_b1 dSQ2r3_dot_b1 dSQ3r3_dot_b1 dSQ4r3_dot_b1
%syms SQr1_dot_b2 dSQ1r1_dot_b2 dSQ2r1_dot_b2 dSQ3r1_dot_b2 dSQ4r1_dot_b2
%syms SQr2_dot_b2 dSQ1r2_dot_b2 dSQ2r2_dot_b2 dSQ3r2_dot_b2 dSQ4r2_dot_b2
%syms SQr3_dot_b2 dSQ1r3_dot_b2 dSQ2r3_dot_b2 dSQ3r3_dot_b2 dSQ4r3_dot_b2
%
%assume([SQr1_dot_b1 dSQ1r1_dot_b1 dSQ2r1_dot_b1 dSQ3r1_dot_b1 dSQ4r1_dot_b1],'real')
%assume([SQr2_dot_b1 dSQ1r2_dot_b1 dSQ2r2_dot_b1 dSQ3r2_dot_b1 dSQ4r2_dot_b1],'real')
%assume([SQr3_dot_b1 dSQ1r3_dot_b1 dSQ2r3_dot_b1 dSQ3r3_dot_b1 dSQ4r3_dot_b1],'real')
%assume([SQr1_dot_b2 dSQ1r1_dot_b2 dSQ2r1_dot_b2 dSQ3r1_dot_b2 dSQ4r1_dot_b2],'real')
%assume([SQr2_dot_b2 dSQ1r2_dot_b2 dSQ2r2_dot_b2 dSQ3r2_dot_b2 dSQ4r2_dot_b2],'real')
%assume([SQr3_dot_b2 dSQ1r3_dot_b2 dSQ2r3_dot_b2 dSQ3r3_dot_b2 dSQ4r3_dot_b2],'real')
%
%
%SQxr1_dot_b1 = SQr1_dot_b1 + dSQ1r1_dot_b1*dq1 + dSQ2r1_dot_b1*dq2 + dSQ3r1_dot_b1*dq3 + dSQ4r1_dot_b1*dq4
%SQxr2_dot_b1 = SQr2_dot_b1 + dSQ1r2_dot_b1*dq1 + dSQ2r2_dot_b1*dq2 + dSQ3r2_dot_b1*dq3 + dSQ4r2_dot_b1*dq4
%SQxr3_dot_b1 = SQr3_dot_b1 + dSQ1r3_dot_b1*dq1 + dSQ2r3_dot_b1*dq2 + dSQ3r3_dot_b1*dq3 + dSQ4r3_dot_b1*dq4
%SQxr1_dot_b2 = SQr1_dot_b2 + dSQ1r1_dot_b2*dq1 + dSQ2r1_dot_b2*dq2 + dSQ3r1_dot_b2*dq3 + dSQ4r1_dot_b2*dq4
%SQxr2_dot_b2 = SQr2_dot_b2 + dSQ1r2_dot_b2*dq1 + dSQ2r2_dot_b2*dq2 + dSQ3r2_dot_b2*dq3 + dSQ4r2_dot_b2*dq4
%SQxr3_dot_b2 = SQr3_dot_b2 + dSQ1r3_dot_b2*dq1 + dSQ2r3_dot_b2*dq2 + dSQ3r3_dot_b2*dq3 + dSQ4r3_dot_b2*dq4
%
%SQx_dot_b1=[SQxr1_dot_b1;SQxr2_dot_b1;SQxr3_dot_b1]
%SQx_dot_b2=[SQxr1_dot_b2;SQxr2_dot_b2;SQxr3_dot_b2]
%
%%collect(simplify([cross(SQx_dot_b1,SQx_dot_b2);dot(SQx_dot_b1,SQx_dot_b2)]),[dq1 dq2 dq3 dq4])
%
%nasty1=simplify(taylor(simplify([cross(SQx_dot_b1,SQx_dot_b2);dot(SQx_dot_b1,SQx_dot_b2)]),[dq1 dq2 dq3 dq4],'ExpansionPoint',[0 0 0 0],'Order',2));
%
%nasty2=simplify(taylor(sum(nasty1.^2),[dq1 dq2 dq3 dq4],'ExpansionPoint',[0 0 0 0],'Order',2));
%
%deltaq1=diff(nasty2,dq1)
%deltaq2=diff(nasty2,dq2)
%deltaq3=diff(nasty2,dq3)
%deltaq4=diff(nasty2,dq4)
%
%[s1;s2;s3;s4]=[cross(SQ*b1,SQ*b2);dot(SQ*b1,SQ*b2)];
%[s1;s2;s3;s4]=[SQ*cross(b1,b2);dot(b1,b2)];
%simplify(sqrt(sum(jacobian(atan2(sqrt(s1^2+s2^2+s3^2),s4),[s1,s2,s3,s4]).^2)))
%1/(s1^2 + s2^2 + s3^2 + s4^2)^(1/2)
%
%collect([deltaq1;deltaq2;deltaq3;deltaq4],[dSQ1r1_dot_b1 dSQ1r1_dot_b2 dSQ1r2_dot_b1 dSQ1r2_dot_b2 dSQ1r3_dot_b1 dSQ1r3_dot_b2 dSQ2r1_dot_b1 dSQ2r1_dot_b2 dSQ2r2_dot_b1 dSQ2r2_dot_b2 dSQ2r3_dot_b1 dSQ2r3_dot_b2 dSQ3r1_dot_b1 dSQ3r1_dot_b2 dSQ3r2_dot_b1 dSQ3r2_dot_b2 dSQ3r3_dot_b1 dSQ3r3_dot_b2 dSQ4r1_dot_b1 dSQ4r1_dot_b2 dSQ4r2_dot_b1 dSQ4r2_dot_b2 dSQ4r3_dot_b1 dSQ4r3_dot_b2])
% 
% 
  
% (2*SQr1_dot_b1*SQr1_dot_b2^2 + 2*SQr1_dot_b1*SQr2_dot_b2^2 + 2*SQr1_dot_b1*SQr3_dot_b2^2)*dSQ1r1_dot_b1 + 
% (2*SQr2_dot_b1*SQr1_dot_b2^2 + 2*SQr2_dot_b1*SQr2_dot_b2^2 + 2*SQr2_dot_b1*SQr3_dot_b2^2)*dSQ1r2_dot_b1 + 
% (2*SQr3_dot_b1*SQr1_dot_b2^2 + 2*SQr3_dot_b1*SQr2_dot_b2^2 + 2*SQr3_dot_b1*SQr3_dot_b2^2)*dSQ1r3_dot_b1 + 
% (2*SQr1_dot_b2*SQr1_dot_b1^2 + 2*SQr1_dot_b2*SQr2_dot_b1^2 + 2*SQr1_dot_b2*SQr3_dot_b1^2)*dSQ1r1_dot_b2 + 
% (2*SQr2_dot_b2*SQr1_dot_b1^2 + 2*SQr2_dot_b2*SQr2_dot_b1^2 + 2*SQr2_dot_b2*SQr3_dot_b1^2)*dSQ1r2_dot_b2 + 
% (2*SQr3_dot_b2*SQr1_dot_b1^2 + 2*SQr3_dot_b2*SQr2_dot_b1^2 + 2*SQr3_dot_b2*SQr3_dot_b1^2)*dSQ1r3_dot_b2
% 
% (2*SQr1_dot_b1*SQr1_dot_b2^2 + 2*SQr1_dot_b1*SQr2_dot_b2^2 + 2*SQr1_dot_b1*SQr3_dot_b2^2)*dSQ2r1_dot_b1 + 
% (2*SQr2_dot_b1*SQr1_dot_b2^2 + 2*SQr2_dot_b1*SQr2_dot_b2^2 + 2*SQr2_dot_b1*SQr3_dot_b2^2)*dSQ2r2_dot_b1 + 
% (2*SQr3_dot_b1*SQr1_dot_b2^2 + 2*SQr3_dot_b1*SQr2_dot_b2^2 + 2*SQr3_dot_b1*SQr3_dot_b2^2)*dSQ2r3_dot_b1 + 
% (2*SQr1_dot_b2*SQr1_dot_b1^2 + 2*SQr1_dot_b2*SQr2_dot_b1^2 + 2*SQr1_dot_b2*SQr3_dot_b1^2)*dSQ2r1_dot_b2 + 
% (2*SQr2_dot_b2*SQr1_dot_b1^2 + 2*SQr2_dot_b2*SQr2_dot_b1^2 + 2*SQr2_dot_b2*SQr3_dot_b1^2)*dSQ2r2_dot_b2 + 
% (2*SQr3_dot_b2*SQr1_dot_b1^2 + 2*SQr3_dot_b2*SQr2_dot_b1^2 + 2*SQr3_dot_b2*SQr3_dot_b1^2)*dSQ2r3_dot_b2
% 
% (2*SQr1_dot_b1*SQr1_dot_b2^2 + 2*SQr1_dot_b1*SQr2_dot_b2^2 + 2*SQr1_dot_b1*SQr3_dot_b2^2)*dSQ3r1_dot_b1 + 
% (2*SQr2_dot_b1*SQr1_dot_b2^2 + 2*SQr2_dot_b1*SQr2_dot_b2^2 + 2*SQr2_dot_b1*SQr3_dot_b2^2)*dSQ3r2_dot_b1 + 
% (2*SQr3_dot_b1*SQr1_dot_b2^2 + 2*SQr3_dot_b1*SQr2_dot_b2^2 + 2*SQr3_dot_b1*SQr3_dot_b2^2)*dSQ3r3_dot_b1 + 
% (2*SQr1_dot_b2*SQr1_dot_b1^2 + 2*SQr1_dot_b2*SQr2_dot_b1^2 + 2*SQr1_dot_b2*SQr3_dot_b1^2)*dSQ3r1_dot_b2 + 
% (2*SQr2_dot_b2*SQr1_dot_b1^2 + 2*SQr2_dot_b2*SQr2_dot_b1^2 + 2*SQr2_dot_b2*SQr3_dot_b1^2)*dSQ3r2_dot_b2 + 
% (2*SQr3_dot_b2*SQr1_dot_b1^2 + 2*SQr3_dot_b2*SQr2_dot_b1^2 + 2*SQr3_dot_b2*SQr3_dot_b1^2)*dSQ3r3_dot_b2
% 
% (2*SQr1_dot_b1*SQr1_dot_b2^2 + 2*SQr1_dot_b1*SQr2_dot_b2^2 + 2*SQr1_dot_b1*SQr3_dot_b2^2)*dSQ4r1_dot_b1 + 
% (2*SQr2_dot_b1*SQr1_dot_b2^2 + 2*SQr2_dot_b1*SQr2_dot_b2^2 + 2*SQr2_dot_b1*SQr3_dot_b2^2)*dSQ4r2_dot_b1 + 
% (2*SQr3_dot_b1*SQr1_dot_b2^2 + 2*SQr3_dot_b1*SQr2_dot_b2^2 + 2*SQr3_dot_b1*SQr3_dot_b2^2)*dSQ4r3_dot_b1 + 
% (2*SQr1_dot_b2*SQr1_dot_b1^2 + 2*SQr1_dot_b2*SQr2_dot_b1^2 + 2*SQr1_dot_b2*SQr3_dot_b1^2)*dSQ4r1_dot_b2 + 
% (2*SQr2_dot_b2*SQr1_dot_b1^2 + 2*SQr2_dot_b2*SQr2_dot_b1^2 + 2*SQr2_dot_b2*SQr3_dot_b1^2)*dSQ4r2_dot_b2 + 
% (2*SQr3_dot_b2*SQr1_dot_b1^2 + 2*SQr3_dot_b2*SQr2_dot_b1^2 + 2*SQr3_dot_b2*SQr3_dot_b1^2)*dSQ4r3_dot_b2
% 
% 2*[SQr1_dot_b2 SQr2_dot_b2 SQr3_dot_b2]*([SQr1_dot_b2;SQr2_dot_b2;SQr3_dot_b2]*[SQr1_dot_b1 SQr2_dot_b1 SQr3_dot_b1])*[dSQ1r1_dot_b1;dSQ1r2_dot_b1;dSQ1r3_dot_b1]
% 2*[SQr1_dot_b1 SQr2_dot_b1 SQr3_dot_b1]*([SQr1_dot_b1;SQr2_dot_b1;SQr3_dot_b1]*[SQr1_dot_b2 SQr2_dot_b2 SQr3_dot_b2])*[dSQ1r1_dot_b2;dSQ1r2_dot_b2;dSQ1r3_dot_b2]
%
% 2*(((SQ*b2)')*((SQ*b2)*((SQ*b1)'))*(dSQ1*b1)+((SQ*b1)')*([SQ*b1]*((SQ*b2)'))*(dSQ1*b2))

%SQxb1=SQ*b1
%SQxb2=SQ*b2
%
%SQa=SQxb2'*SQxb2*SQxb1';
%SQb=SQxb1'*SQxb1*SQxb2';
%2*(SQa*dSQ1*b1+SQb*dSQ1*b2)
%2*(SQa*dSQ2*b1+SQb*dSQ2*b2)
%2*(SQa*dSQ3*b1+SQb*dSQ3*b2)
%2*(SQa*dSQ4*b1+SQb*dSQ4*b2)

%SQx is attitude matrix, |b1| = 1, |b2| = 1

%SQa=SQxb1';
%SQb=SQxb2';

%SQa=b1'*SQ';
%SQb=b2'*SQ';


2*(b1'*SQ'*dSQ1*b1+b2'*SQ'*dSQ1*b2)
%2*(b1'*SQ'*dSQ2*b1+b2'*SQ'*dSQ2*b2)
%2*(b1'*SQ'*dSQ3*b1+b2'*SQ'*dSQ3*b2)
%2*(b1'*SQ'*dSQ4*b1+b2'*SQ'*dSQ4*b2) 
%
%1/((SQr1_dot_b1^2 + SQr2_dot_b1^2 + SQr3_dot_b1^2)^(1/2)*(SQr1_dot_b2^2 + SQr2_dot_b2^2 + SQr3_dot_b2^2)^(1/2))
 


