function [params,P,I_stats] = FastSExtractorDecoupled( I )

TileSize=64;    %according to SExtractor documentation 32-256 works well.
BrightestN = 10;%use the N brightest stars that meet all of the criteria. 10 seems to be the best

[ params, I_stats ] = ExtractStarsAndStats( I, TileSize, BrightestN  );
P=[];
% tic
stars=params_and_image_to_stars(params, I);
for i=1:5
	[ H, predicted_val, observed_val,  observed_var, params, ~, residual, stars] = multiparam_pixval_predictor_decoupled( params, stars, I_stats );
	[params,P] = weighted_least_squares( params, observed_val(:), sparse_diag(observed_var(:)), predicted_val,H,1);
end
[ H, predicted_val, observed_val,  observed_var, params, params_idx, residual, stars] = multiparam_pixval_predictor_decoupled( params, stars, I_stats );
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

