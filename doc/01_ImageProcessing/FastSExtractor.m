function [params,P,I_stats] = FastSExtractor( I )


ConcavityThreshold=0.15;%minimum concavity of the central 3x3 pixel window.
                      %0.1-0.25 was experementally determined to reject
                      %image artifacts over a large variety of source
                      %images. If this is too close to zero, the psf_width
                      %solution may diverge
					  
TileSize=64;    %according to SExtractor documentation 32-256 works well.
BrightestN = 10;%use the N brightest stars that meet all of the criteria. 10 seems to be the best

[I_grey,I_stats]=CollectImageStats(I);

aa=bwconncomp((I_grey>imresize(I_stats.m_img+5*sqrt(I_stats.v_img),size(I_grey),'bilinear')),4);
stars = zeros(aa.NumObjects,4);
star_idx=1;
% tic
for i=aa.PixelIdxList
    idx=cell2mat(i);
	%note: Matlab stores images in column major order
    [Px,Py]=idx2xy(idx, size(I_grey,1));
    %if (min(Px)<max(Px)&&min(Py)<max(Py))
	if numel(idx)>1
		P=[Py,Px];

		[bg_val,~,~]=windowed_mvp(Px, Py, I_stats);
		val=I_grey(idx)-bg_val;
		r_val=[val val];
		totalval=sum(val);
		
		%py,px
		centroid     = sum(r_val.*P,1)/totalval; r_centroid=repmat(centroid,size(val));
		
		%Check that the 3x3 windowed region is concave
		[ sum_window ,x_concavity, y_concavity ] = centroid_concavity(centroid , I_grey, I_stats );
		%assert(all(floor(centroid)==[131 1273])==0)
		%if sum_window>0&&x_concavity+y_concavity>sum_window*ConcavityThreshold
			%trace of moment covarience ~= 2*psf_radius^2 
			v  = ((P-r_centroid).*r_val)'*(P-r_centroid)/totalval;
			tr = trace(v);
			min_eig=(tr-sqrt(max(tr^2-4*det(v),0)))/2;
			stars(star_idx,:)=[centroid,totalval,min_eig];
			star_idx = star_idx+1;
			
% 			II(idx)=val;
		%end
	end
end
% toc
stars=sortrows(stars(1:(star_idx-1),:),3,'descend');

stars=stars(1:min(BrightestN,size(stars,1)),:);

params=stars(:,[2 1 3])';
format short g
%params {x1 y1 totalval1 ... xN yN totalvalN psf_radius}
params=[params(:)' sqrt(max(mean(stars(:,4)),1/12))];
P=[];
% tic
for i=1:5
	
	[ H, predicted_val, observed_val,  observed_var, params, ~, residual] = multiparam_pixval_predictor( params, I_grey, I_stats );
	[params,P] = weighted_least_squares( params, observed_val(:), sparse_diag(observed_var(:)), predicted_val,H,1);
end

[ H, predicted_val, observed_val,  observed_var, params, params_idx, residual] = multiparam_pixval_predictor( params, I_grey, I_stats );
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

