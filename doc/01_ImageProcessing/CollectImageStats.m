function [ I_grey, I_stats ] = CollectImageStats(I)

	TileSize=64;    %according to SExtractor documentation 32-256 works well.

	img_height=size(I,1);
	img_width =size(I,2);
	channels=size(I,3);
	step=1/double(256*channels-1);
	I_grey=sum(im2double(I),3)/double(channels);%I/255

	m_img=zeros(ceil(size(I_grey)/TileSize));
	v_img=zeros(size(m_img));
	p_img=zeros(size(m_img));
	y_idx=round(linspace(1,size(I_grey,1)+1,size(m_img,1)+1));
	x_idx=round(linspace(1,size(I_grey,2)+1,size(m_img,2)+1));
	for y=1:size(m_img,1)
		for x=1:size(m_img,2)
       
			[bin_count,bin_val]=imhist(I_grey(y_idx(y):y_idx(y+1)-1,x_idx(x):x_idx(x+1)-1),256*channels);
			old_idx=-1;
			idx=numel(bin_count);
			%numiter=0;
			while idx ~= old_idx
				m=sum(bin_val(1:idx).*bin_count(1:idx))/sum(bin_count(1:idx));
				v=sum((bin_val(1:idx)-m).^2.*bin_count(1:idx))/(sum(bin_count(1:idx))-1);
				old_idx=idx;
				idx=find(bin_val>(m+5*sqrt(v)),1)-1;
				%numiter=numiter+1;
			end
			%numiter
			%bias assuming uniform photon distribution within bins
			p=(v-step^2/12)/(m+step/2);
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
