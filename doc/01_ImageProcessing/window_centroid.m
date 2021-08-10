function [ windowed_val,windowed_var ] = window_centroid(star,I_stats)
%   Detailed explanation goes here
	[w_bg_val,w_bg_v,w_bg_p]=windowed_mvp(star.Pwx, star.Pwy,I_stats);
	windowed_val=star.val-w_bg_val;
	windowed_var=max(star.val.*w_bg_p,0)+w_bg_v;
	
end

	
