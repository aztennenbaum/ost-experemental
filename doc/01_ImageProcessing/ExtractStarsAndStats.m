function [ params, I_stats ] = ExtractStarsAndStats( I, TileSize, BrightestN  )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

ConcavityThreshold=0.15;%minimum concavity of the central 3x3 pixel window.
                      %0.1-0.25 was experementally determined to reject
                      %image artifacts over a large variety of source
                      %images. If this is too close to zero, the psf_width
                      %solution may diverge
					  

[I_stats]=CollectImageStats(I,TileSize);

threshold_map=single(I_stats.m_img+5*sqrt(I_stats.v_img));
threshold_map_large=imresize(threshold_map,size(I),'bilinear');
aa=bwconncomp(I>threshold_map_large,4);
params = zeros(aa.NumObjects,4);
star_idx=1;
% tic
for pixel=aa.PixelIdxList
    idx=cell2mat(pixel);
	%note: Matlab stores images in column major order
    [Px,Py]=idx2xy(idx, size(I,1));
    if (min(Px)<max(Px)&&min(Py)<max(Py))
		P=[Py,Px];

		%[bg_val,~,~]=windowed_mvp(Px, Py, I_stats);
		val=double(single(I(idx))-threshold_map_large(idx));
		r_val=[val val];
		totalval=sum(val);
		
		centroid     = sum(r_val.*P,1)/totalval; r_centroid=repmat(centroid,size(val));
        
		v  = ((P-r_centroid).*r_val)'*(P-r_centroid)/totalval;
		tr = trace(v);
		min_eig=(tr-sqrt(max(tr^2-4*det(v),0)))/2;
		params(star_idx,:)=[centroid,totalval,min_eig];
		star_idx = star_idx+1;
			
    end
end
% toc
params=sortrows(params(1:(star_idx-1),:),3,'descend');

params=params(1:min(BrightestN,size(params,1)),:);

if numel(params)>0
    stars2=starExtractorTest(I,threshold_map_large);stars2=double(stars2(:,1:min(end,BrightestN))');
    I_red=I;I_green=I;I_blue=I;
    I_red(get_star_pixels_idx(params(:,2),params(:,1),5,size(I,2),size(I,1)))=I_stats.img_max;
    I_green(get_star_pixels_idx(stars2(:,2),stars2(:,1),3,size(I,2),size(I,1)))=I_stats.img_max;
    imshow(cat(3,I_red,I_green,I_blue));
    figure
    imshow(double(threshold_map_large)/I_stats.img_max)
    figure
    imshow(double(interpolateThresholdTest(I,threshold_map))/I_stats.img_max)
    figure
    [t,~,~]=estimateBackgroundTest(I,threshold_map);
    imshow(double(interpolateThresholdTest(I,t))/I_stats.img_max);
    max(max(threshold_map_large-interpolateThresholdTest(I,threshold_map)))
    keyboard
end

params=params(:,[2 1 3])';
%params {x1 y1 totalval1 ... xN yN totalvalN psf_radius}
params=[params(:)' ];

end

