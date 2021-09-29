function [ windowed_val,windowed_var ] = window_centroid(star,I_stats)
%   Detailed explanation goes here
    val=double(star.val);
	[w_bg_val,w_bg_v,w_bg_p]=windowed_mvp(star.Px, star.Py,I_stats);
	windowed_val=val-w_bg_val;
	windowed_var=max(val.*w_bg_p,0)+w_bg_v;
	
end

	
