function [ H, predicted_val, observed_val,  observed_var, params_new, params_idx, residual_new] = multiparam_pixval_predictor( params, I_grey, I_stats )
%todo better way to do this (pixelval and jacobian output number of nonzero
%rows, dont change size of matrices
%UNTITLED14 Summary of this function goes here
%   Detailed explanation goes here]

	row_offset=0;
	col_offset=0;
	if numel(params)>1
		x_ctr      = params(1:3:end-1);
		y_ctr      = params(2:3:end-1);
		
		psf_radius = max(params(end),sqrt(1/12));

		[ x_min, x_max, y_min, y_max ] = windowed_centroid_bounds( x_ctr, y_ctr ,2 , I_stats.img_width, I_stats.img_height);
		pixel_count_total=sum((x_max-x_min).*(y_max-y_min));

		N=floor(numel(params)/3);
		H=spalloc(pixel_count_total,3*N+1,4*pixel_count_total);

		params_new     = zeros(size(params));
		params_idx     = zeros(size(params));
		predicted_val  = zeros(pixel_count_total,1);
		observed_val   = zeros(pixel_count_total,1);
		observed_var   = zeros(pixel_count_total,1);


		for i=1:N
			centroid   = params(3*(i-1)+[2 1]);
			star=stats2star(centroid ,2, I_grey);
			[observed_val_part,observed_var_part]=window_centroid(star,I_stats );
			[predicted_val_part,predicted_H_part]=pixelval_and_jacobian( [params(3*(i-1)+(1:3)) psf_radius], star.Px, star.Py);

			%exclude pixels which are predicted to be over the max value, or
			%smaller than maxval*eps
			val_idx   = and(predicted_val_part<I_stats.img_max,predicted_val_part>I_stats.img_max*eps);
			val_count = sum(val_idx);


			if nnz(observed_var_part(val_idx))==val_count &&rcond(predicted_H_part(val_idx,:)'*diag(1./observed_var_part(val_idx))*predicted_H_part(val_idx,:))>eps
				H(row_offset+(1:val_count),end)=predicted_H_part(val_idx,end);
				H(row_offset+(1:val_count),col_offset+(1:3))=predicted_H_part(val_idx,1:3);

				params_new(col_offset+(1:3)) = params(3*(i-1)+(1:3));
				params_idx(col_offset+(1:3)) = (3*(i-1)+(1:3));

				predicted_val(row_offset+(1:val_count))=predicted_val_part(val_idx);
				observed_val(row_offset+(1:val_count))=observed_val_part(val_idx);
				observed_var(row_offset+(1:val_count))=observed_var_part(val_idx);
				row_offset=row_offset+val_count;
				col_offset=col_offset+3;

			end
		end
	end
	if col_offset>0
		
		H=H(1:row_offset,[1:col_offset end]);
		
		predicted_val   = predicted_val(1:row_offset);
		observed_val    = observed_val(1:row_offset);
		observed_var    = observed_var(1:row_offset);
		
		params_new      = params_new(1:(col_offset+1));
		params_idx      = params_idx(1:(col_offset+1));

		params_new(end) = psf_radius;
		params_idx(end) = numel(params) ;
		
		y = observed_val - predicted_val;
		residual_new=y'*((1./observed_var).*y);
	else
		H=[];
		predicted_val=[];
		observed_val=[];
		observed_var=[];
		params_new=[];
		params_idx=[];
		residual_new=0;
	end
	
end
