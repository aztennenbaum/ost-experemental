function [param_new,P] = weighted_least_squares( param, observed_val, R, predicted_val,H,alpha)
%UNTITLED10 Summary of this function goes here
%   Detailed explanation goes here

y = observed_val - predicted_val;
residual=(y'/R)*y;
Ht_Rinv=H'/R;
if numel(H)>0
    P = (Ht_Rinv*H)^-1;
else
    P=[];
end
k = P*Ht_Rinv;
param_new = param + alpha*(k*y)';
end
