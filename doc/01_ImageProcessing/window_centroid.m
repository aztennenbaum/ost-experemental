function [ windowed_val,windowed_var,Pwx,Pwy ] = window_centroid(centroid ,r, I,I_stats)
%   Detailed explanation goes here

	x_ctr=centroid(2);
	y_ctr=centroid(1);

	[ x_min, x_max, y_min, y_max ] = windowed_centroid_bounds( x_ctr, y_ctr,r, size(I,2) , size(I,1) );
	
	[Pwx,Pwy]=meshgrid(x_min:x_max,y_min:y_max);
	[w_bg_val,w_bg_v,w_bg_p]=windowed_mvp(Pwx, Pwy,I_stats);
	windowed_val=I(y_min:y_max,x_min:x_max)-w_bg_val;
	windowed_var=max(I(y_min:y_max,x_min:x_max).*w_bg_p,0)+w_bg_v;
	
end

	
