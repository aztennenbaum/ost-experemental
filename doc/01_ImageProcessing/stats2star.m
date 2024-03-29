function [ star ] = stats2star(centroid ,r, I)
%   Detailed explanation goes here

	x_ctr=centroid(2);
	y_ctr=centroid(1);

	[ x_min, x_max, y_min, y_max ] = windowed_centroid_bounds( x_ctr, y_ctr,r, size(I,2) , size(I,1) );
	
	[Px,Py]=meshgrid(x_min:x_max,y_min:y_max);
    val=I(y_min:y_max,x_min:x_max);
   	star=struct('Px',Px(:),...
		        'Py',Py(:),...
				'val',val(:));
	
end

	
