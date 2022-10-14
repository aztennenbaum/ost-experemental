function [ centroid,totalval,min_eig ] = star2stats( star )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
		P=[star.Py,star.Px];

		%[bg_val,~,~]=windowed_mvp(Px, Py, I_stats);
		r_val=[star.val star.val];
		totalval=sum(star.val);
		
		centroid     = sum(r_val.*P,1)/totalval; r_centroid=repmat(centroid,size(star.val));
        
		v  = ((P-r_centroid).*r_val)'*(P-r_centroid)/totalval;
		tr = trace(v);
		min_eig=(tr-sqrt(max(tr^2-4*det(v),0)))/2;
end

