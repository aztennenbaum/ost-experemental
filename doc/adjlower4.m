function [ out ] = adjlower4( in )
a1=in(1,1);
b1=in(2,1); b2=in(2,2);
c1=in(3,1); c2=in(3,2); c3=in(3,3);
d1=in(4,1); d2=in(4,2); d3=in(4,3); d4=in(4,4);

out=[[                             b2*c3*d4,                   0,         0,        0];...
[                                 -b1*c3*d4,            a1*c3*d4,         0,        0];...
[                       b1*c2*d4 - b2*c1*d4,           -a1*c2*d4,  a1*b2*d4,        0];...
[ b1*c3*d2 - b1*c2*d3 + b2*c1*d3 - b2*c3*d1, a1*c2*d3 - a1*c3*d2, -a1*b2*d3, a1*b2*c3]];

end

