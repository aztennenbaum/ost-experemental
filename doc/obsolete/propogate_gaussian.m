function [ cf_x,cf_v ] = propogate_gaussian( c,f_x,x,v )

J=jacobian(f_x,x);
cf_v=c*(J*v*J')*c';
cf_x=c*f_x;
end

