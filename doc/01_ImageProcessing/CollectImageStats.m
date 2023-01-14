function [ I_stats ] = CollectImageStats(I,I_config)
	img_height=size(I,1);
	img_width =size(I,2);
	[~,bin_val]=imhist(I);
	img_max=bin_val(end);

	m_img=single(zeros(I_config.map_height,I_config.map_width));
	v_img=single(zeros(size(m_img)));
	p_img=single(zeros(size(m_img)));
	y_idx=round(linspace(1,size(I,1)+1,size(m_img,1)+1));
	x_idx=round(linspace(1,size(I,2)+1,size(m_img,2)+1));
	for y=1:size(m_img,1)
		for x=1:size(m_img,2)
            tile=I(y_idx(y):y_idx(y+1)-1,x_idx(x):x_idx(x+1)-1);
            [bin_count,bin_val]=imhist(tile);
            [ bg_mean, bg_var ] = calculate_bg_stats( bin_count, bin_val, I_config.threshold_sigma);
            
            m_img(y,x)=bg_mean;%mean
            v_img(y,x)=bg_var;%varience
            p_img(y,x)=bg_var/bg_mean;%photons2val
		end
	end

	I_stats=struct('m_img',m_img,...
		           'v_img',v_img,...
				   'p_img',p_img,...
				   'x_idx',x_idx,...
				   'y_idx',y_idx,...
				   'step',bin_val(2)-bin_val(1),...
				   'img_max',img_max,...
				   'img_height',img_height,...
				   'img_width',img_width);
end
