function [ params, I_stats, stars ] = ExtractStarsAndStats( I, I_config  )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
BrightestN=I_config.max_stars;


[I_stats]=CollectImageStats(I,I_config.map_height,I_config.map_width);

threshold_map=I_stats.m_img+5*sqrt(I_stats.v_img);
%threshold_map_large=imresize(threshold_map,size(I),'bilinear');
m_large=imresize(I_stats.m_img,size(I),'bilinear');
v_large=imresize(I_stats.v_img,size(I),'bilinear');
threshold_map_large=m_large+5*sqrt(v_large);
aa=bwconncomp(I>threshold_map_large,4);
S_stats = zeros(aa.NumObjects,4);
star_idx=1;
% tic
for pixel=aa.PixelIdxList
    idx=cell2mat(pixel);
	%note: Matlab stores images in column major order
    [Px,Py]=idx2xy(idx, size(I,1));
    if (min(Px)<max(Px)&&min(Py)<max(Py))
	%if numel(idx)>1
        [bg_val,~,~]=windowed_mvp(Px, Py, I_stats);
        val=double(I(idx))-bg_val;
        star=struct('Px',Px(:),'Py',Py(:),'val',val(:));
        [centroid,totalval,min_eig]=star2stats(star);
		S_stats(star_idx,:)=[centroid,totalval,min_eig];
		star_idx = star_idx+1;
			
    end
end
% toc
S_stats=sortrows(S_stats(1:(star_idx-1),:),3,'descend');

S_stats=S_stats(1:min(BrightestN,size(S_stats,1)),:);

if numel(S_stats)>0
    stars2=starExtractorTest(I,I_stats.m_img,I_stats.v_img);stars2=double(stars2(:,1:min(end,BrightestN))');
    I_red=I;I_green=I;I_blue=I;
    I_red(get_star_pixels_idx(S_stats(:,2),S_stats(:,1),5,I_stats.img_width, I_stats.img_height))=I_stats.img_max;
    I_green(get_star_pixels_idx(stars2(:,2),stars2(:,1),3,I_stats.img_width, I_stats.img_height))=I_stats.img_max;
    imshow(cat(3,I_red,I_green,I_blue));
    figure
    imshow(double(threshold_map_large)/I_stats.img_max)
    %figure
    %imshow(double(interpolateMapTest(threshold_map,I_config))/I_stats.img_max)
%     figure
%     [m,v]=estimateBackgroundTest(I,threshold_map);
%     t=m+5*sqrt(v);
%     imshow(double(interpolateMapTest(I,t))/I_stats.img_max);
%     max(max(threshold_map_large-interpolateMapTest(I,threshold_map)))
    keyboard
end

params=S_stats(:,[2 1 3])';
%params {x1 y1 totalval1 ... xN yN totalvalN psf_radius}
params=[params(:)' sqrt(max(mean(S_stats(:,4)),1/12))];

stars=stats_and_image_to_stars(S_stats, I, I_config.r);
end

