function [ out ] = rotvec( in )
vec=[0 0 1];
out=[cross(vec,in),dot(vec,in)+1];
end
