function [params,P,I_stats,debug] = FastSExtractorDecoupled( I )

ConcavityThreshold=0.15;%minimum concavity of the central 3x3 pixel window.
                      %0.1-0.25 was experementally determined to reject
                      %image artifacts over a large variety of source
                      %images. If this is too close to zero, the psf_width
                      %solution may diverge
                      
TileSize = 64;  %according to SExtractor documentation 32-256 works well.
BrightestN = 10;%use the N brightest stars that meet all of the criteria. 10 seems to be the best
r = 2; %pixel sample radui - sample pixels in a d^2 square centered at the
       %middle of the threshold pixels, where d=2*r+1
img_height=size(I,1);
img_width =size(I,2);

I_config=struct('ConcavityThreshold',ConcavityThreshold,...
	           'TileSize',TileSize,...
			   'BrightestN',BrightestN,...
			   'r',r,...
			   'img_height',img_height,...
			   'img_width',img_width);


[ params, I_stats, stars ] = ExtractStarsAndStats( I, I_config  );
debug=params;
P=[];
% tic
for i=1:5
	[ H, predicted_val, observed_val,  observed_var, params, ~, residual, stars] = multiparam_pixval_predictor_decoupled( params, I_stats, stars);
	[params,P] = weighted_least_squares( params, observed_val(:), sparse_diag(observed_var(:)), predicted_val,H,1);
end
[ H, predicted_val, observed_val,  observed_var, params, params_idx, residual, stars] = multiparam_pixval_predictor_decoupled( params, I_stats, stars );
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

