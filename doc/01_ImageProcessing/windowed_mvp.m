function [ m_windowed, v_windowed, p_windowed] = windowed_mvp( Px, Py, I_stats)
	%UNTITLED9 Summary of this function goes here
	%   Detailed explanation goes here

	x_interp=(I_stats.x_idx(2:end)+I_stats.x_idx(1:end-1))/2;
	y_interp=(I_stats.y_idx(2:end)+I_stats.y_idx(1:end-1))/2;

	Px_clipped=min(max(Px,x_interp(1)),x_interp(end));
	Py_clipped=min(max(Py,y_interp(1)),y_interp(end));

	m_windowed=interp2(x_interp,y_interp,I_stats.m_img,Px_clipped,Py_clipped);
	v_windowed=interp2(x_interp,y_interp,I_stats.v_img,Px_clipped,Py_clipped);
	p_windowed=interp2(x_interp,y_interp,I_stats.p_img,Px_clipped,Py_clipped);

end

