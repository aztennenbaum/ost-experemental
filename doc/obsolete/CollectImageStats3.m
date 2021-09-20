function [ I_grey, I_stats ] = CollectImageStats3(I,TileSize)

	img_height=size(I,1);
	img_width =size(I,2);
	channels=size(I,3);
	step=1/double(256*channels-1);
	I_grey=min(sum(im2double(I),3)/double(channels),1);%I/255

	m_img=zeros(ceil(size(I_grey)/TileSize));
	v_img=zeros(size(m_img));
	p_img=zeros(size(m_img));
	y_idx=round(linspace(1,size(I_grey,1)+1,size(m_img,1)+1));
	x_idx=round(linspace(1,size(I_grey,2)+1,size(m_img,2)+1));
	for y=1:size(m_img,1)
		for x=1:size(m_img,2)
            tile=I_grey(y_idx(y):y_idx(y+1)-1,x_idx(x):x_idx(x+1)-1);
            s=numel(tile);
            m=mean(tile(:));
            v=var(tile(:));
			%[bin_count,bin_val]=imhist(tile,256);
            %bin_count=bin_count(1:end-1);
            %bin_val=bin_val(1:end-1);
           
            %m=sum(bin_count.*bin_val)/s+1/2;
            %v=sum(bin_count.*bin_val.^2)/s - (m-1/2)^2 + 1/12;
            
            p=v/m;
            
            %t2s=p*(erf(sqrt(m/2)) + 1)/2;
            %totalval=s/t2s;
			
			m_img(y,x)=m;%mean
			v_img(y,x)=v;%varience
			p_img(y,x)=p;%photons2val
		end
	end

	I_stats=struct('m_img',m_img,...
		           'v_img',v_img,...
				   'p_img',p_img,...
				   'x_idx',x_idx,...
				   'y_idx',y_idx,...
				   'step',step,...
				   'img_height',img_height,...
				   'img_width',img_width);
end
