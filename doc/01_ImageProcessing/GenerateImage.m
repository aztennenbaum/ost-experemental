function [ I_grey ] = GenerateImage( params, I_stats )
%GENERATEIMAGE Summary of this function goes here
%   Detailed explanation goes here

N=floor(numel(params)/3);
I_grey=imresize(I_stats.m_img,[I_stats.img_height,I_stats.img_width],'bilinear');
photons2val=imresize(I_stats.p_img,size(I_grey),'bilinear');
psf_radius=params(end);
for i=1:N
	x_ctr   = params(3*(i-1)+1);
	y_ctr   = params(3*(i-1)+2);
	
	[ x_min, x_max, y_min, y_max ] = windowed_centroid_bounds( x_ctr, y_ctr,6, I_stats.img_width , I_stats.img_height );
	
	[Pwx,Pwy]=meshgrid(x_min:x_max,y_min:y_max);
	[val]=pixelval( [params(3*(i-1)+(1:3)) psf_radius], Pwx(:), Pwy(:));
	idx=xy2idx(Pwx(:), Pwy(:),I_stats.img_height);
	I_grey(idx)=I_grey(idx)+val;
end
I_grey=max(min(poissrnd(I_grey./photons2val).*photons2val,1),0);
end

