function [params,P,I_stats,debug] = FastSExtractor( I )

I_config=default_SE_config(I);


max_stars = I_config.max_stars;%use the N brightest stars that meet all of the criteria. 10 seems to be the best

[I_stats]=CollectImageStats(I,I_config.map_height,I_config.map_width);

aa=bwconncomp((I>imresize(I_stats.m_img+5*sqrt(I_stats.v_img),size(I),'bilinear')),4);
S_stats = zeros(aa.NumObjects,4);
star_idx=1;
% tic
for i=aa.PixelIdxList
    idx=cell2mat(i);
	%note: Matlab stores images in column major order
    [Px,Py]=idx2xy(idx, size(I,1));
    if (min(Px)<max(Px)&&min(Py)<max(Py))
	%if numel(idx)>1
		P=[Py,Px];

		[bg_val,~,~]=windowed_mvp(Px, Py, I_stats);
		val=double(I(idx))-bg_val;
		r_val=[val val];
		totalval=sum(val);
		
		%py,px
		centroid     = sum(r_val.*P,1)/totalval; r_centroid=repmat(centroid,size(val));
		
		%Check that the 3x3 windowed region is concave
		%[ sum_window ,x_concavity, y_concavity ] = centroid_concavity(centroid , I, I_stats );
		%assert(all(floor(centroid)==[131 1273])==0)
		%if sum_window>0&&x_concavity+y_concavity>sum_window*ConcavityThreshold
			%trace of moment covarience ~= 2*psf_radius^2 
			v  = ((P-r_centroid).*r_val)'*(P-r_centroid)/totalval;
			tr = trace(v);
			min_eig=(tr-sqrt(max(tr^2-4*det(v),0)))/2;
			S_stats(star_idx,:)=[centroid,totalval,min_eig];
			star_idx = star_idx+1;
			
% 			II(idx)=val;
		%end
	end
end
% toc
S_stats=sortrows(S_stats(1:(star_idx-1),:),3,'descend');

S_stats=S_stats(1:min(max_stars,size(S_stats,1)),:);
params=S_stats(:,[2 1 3])';
format short g
%params {x1 y1 totalval1 ... xN yN totalvalN psf_radius}
params=[params(:)' sqrt(max(mean(S_stats(:,4)),1/12))];
debug=params;
P=[];
% tic
for i=1:5
	
	[ H, predicted_val, observed_val,  observed_var, params, ~, residual] = multiparam_pixval_predictor( params, I, I_stats );
	[params,P] = weighted_least_squares( params, observed_val(:), sparse_diag(observed_var(:)), predicted_val,H,1);
end

[ H, predicted_val, observed_val,  observed_var, params, params_idx, residual] = multiparam_pixval_predictor( params, I, I_stats );
P=P(params_idx,params_idx);
% toc
% [ H, predicted_val, observed_val,  observed_var, params,residual] = multiparam_pixval_predictor( params,maxval, I, x_idx, y_idx,m_img,v_img,p_img );
[params' sqrt(full(diag(P)))]
% figure;imagesc(full(chol(P)))
% figure;imagesc(1./p_img)
% figure;imagesc(1./m_img)
% figure;imagesc(1./v_img)
% figure;imshow(I)
% objs=im2double(imread('img6-indx.png'));
% objs(:,:,3)=objs(:,:,3)+double(III>0);
% objs(:,:,1)=objs(:,:,1)+double(III>0);
% objs(:,:,2)=objs(:,:,2)-double(III>0);
% figure;imshow(objs);
% %figure;imshow(double(III));
% %keyboard;
end

