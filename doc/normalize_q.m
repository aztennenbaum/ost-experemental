function [ out ] = normalize_q( in )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
%[val,idx]=max(sum(input_args.^2,2));
%output_args=sign(input_args(idx,4))*input_args(idx,:)./sqrt(val);
out=normalize_qa(q_max(q_max(q_append_norm_sq(in(1,:)),...
                             q_append_norm_sq(in(2,:))),...
		               q_max(q_append_norm_sq(in(3,:)),...
                             q_append_norm_sq(in(4,:)))));
end

