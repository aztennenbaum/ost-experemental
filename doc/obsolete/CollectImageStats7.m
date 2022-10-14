function [ I_grey, I_stats ] = CollectImageStats7(I,TileSize)

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
            
            s=sum(bin_count);
            m=sum(bin_count.*bin_val)/s+step/2;
            v=sum(bin_count.*bin_val.^2)/s - (m-step/2)^2 + step^2/12;
            
            p=v/m;

            m_orig=m;
            s_orig=s;
            v_orig=v;
            
            nz_idx=(bin_count>0);
            if(sum(nz_idx)>2)
                bin_count=bin_count(nz_idx);
                bin_val=bin_val(nz_idx);
                observed_val=bin_count;
                %observed_var=(diag(bin_count)-bin_count'*bin_count/sum(bin_count)^1);
                observed_var=sparse_diag(bin_count);

                params=[m,s];
                [predicted_val,H]=histval_bg_and_jacobian( params, bin_val,step);
                rank(H)
                [params,P] = weighted_least_squares( params, observed_val(:), observed_var, predicted_val,H,1);
                %[params,P] = weighted_least_squares( params, observed_val(:), eye(numel(observed_var)), predicted_val,H,1);
                m = params(1);
                s = params(2);
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
