function [ I_stats ] = CollectImageStats(I,TileSize)
	img_height=size(I,1);
	img_width =size(I,2);
	[~,bin_val]=imhist(I);
	step=bin_val(2);
	img_max=bin_val(end);

	m_img=zeros(ceil(size(I)/TileSize));
	v_img=zeros(size(m_img));
	p_img=zeros(size(m_img));
	y_idx=round(linspace(1,size(I,1)+1,size(m_img,1)+1));
	x_idx=round(linspace(1,size(I,2)+1,size(m_img,2)+1));
	for y=1:size(m_img,1)
		for x=1:size(m_img,2)
            tile=I(y_idx(y):y_idx(y+1)-1,x_idx(x):x_idx(x+1)-1);
            [bin_count,bin_val]=imhist(tile);

            bin_count=bin_count+eps;
            c0 = bin_count;
            c1 = bin_count.*bin_val;
            c2 = bin_count.*bin_val.^2;

            s=zeros(size(bin_val));
            m=zeros(size(bin_val));
            v=zeros(size(bin_val));
            t=zeros(size(bin_count));
            
            for i=1:numel(bin_count)
              if i>1&&bin_val(i-1)<t(i-1)
                  c0(i)=c0(i)+c0(i-1);
                  c1(i)=c1(i)+c1(i-1);
                  c2(i)=c2(i)+c2(i-1);
              end
              s(i) = c0(i);
              m(i) = c1(i)/c0(i);
              v(i) = c2(i)/c0(i) - m(i)*m(i) + step^2/12;
              m(i) = m(i) + step/2;
              t(i) = m(i) + sqrt(v(i))*5;
            end
            [~,idx]=max(c0);
            m_img(y,x)=m(idx);%mean
            v_img(y,x)=v(idx);%varience
            p_img(y,x)=v(idx)/m(idx);%photons2val
		end
	end

	I_stats=struct('m_img',m_img,...
		           'v_img',v_img,...
				   'p_img',p_img,...
				   'x_idx',x_idx,...
				   'y_idx',y_idx,...
				   'step',step,...
				   'img_max',img_max,...
				   'img_height',img_height,...
				   'img_width',img_width);
end
