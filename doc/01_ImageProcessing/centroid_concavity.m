function [ sum_window ,x_concavity, y_concavity ] = centroid_concavity(centroid , I_grey, I_stats)
%UNTITLED17 Summary of this function goes here
%   Detailed explanation goes here

	center_pixel = max(min(size(I_grey)-1,round(centroid)),2);
	wx=(center_pixel(2)-1):(center_pixel(2)+1);
	wy=(center_pixel(1)-1):(center_pixel(1)+1);
	[Pwx,Pwy]=meshgrid(wx,wy);
	[w_bg_val,w_bg_v,w_bg_p]=windowed_mvp(Pwx, Pwy, I_stats);
	windowed_val=I_grey(wy,wx)-w_bg_val;
	windowed_var=max(I_grey(wy,wx).*w_bg_p,0)+w_bg_v;
	
	sum_window=sum(windowed_val(:));
	x_concavity=sum(windowed_val*[-1;2;-1]);
	y_concavity=sum([-1 2 -1]*windowed_val);
end

