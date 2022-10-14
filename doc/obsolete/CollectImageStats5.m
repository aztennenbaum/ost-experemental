function [ I_grey, I_stats ] = CollectImageStats5(I,TileSize)

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
			[bin_count,bin_val]=imhist(tile,256);
            
            %Stupid hack
            bin_count=bin_count(1:end-1);
            bin_val=bin_val(1:end-1);
            
            s=sum(bin_count);
            m=sum(bin_count.*bin_val)/s+step/2;
            v=sum(bin_count.*bin_val.^2)/s - (m-step/2)^2 + step^2/12;
            
            p=v/m;

            m_orig=m;
            p_orig=p;
            v_orig=v;
            
            nz_idx=(bin_count>0);
            if(sum(nz_idx)>3)

                observed_val=bin_count(nz_idx);
                observed_var=bin_count(nz_idx);
                
                
                t2s=p*(erf(sqrt(m/2)) + 1)/2
                params=[m,p];
                %[predicted_val,H]=histval_and_jacobian( params, bin_val(nz_idx),s/t2s,step);
                [params,P] = weighted_least_squares( params, observed_val(:), sparse_diag(observed_var(:)), predicted_val,H,1);
                [params,P] = weighted_least_squares( params, observed_val(:), eye(numel(observed_var)), predicted_val,H,1);
                m = params(1);
                p = params(2);
                v=m*p;
                
                (m_orig+5*sqrt(v_orig))/step
                (m+5*sqrt(v))/step
            end
            
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
