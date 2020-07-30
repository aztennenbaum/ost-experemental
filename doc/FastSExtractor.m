function [stars] = FastSExtractor( I )
channels=size(I,3);
TileSize=64; %according to SExtractor documentation 32-256 works well.

I=mean(I,3)/256;
m_img=zeros(ceil(size(I)/TileSize));
v_img=zeros(size(m_img));
p_img=zeros(size(m_img));
y_idx=round(linspace(1,size(I,1)+1,size(m_img,1)+1));
x_idx=round(linspace(1,size(I,2)+1,size(m_img,2)+1));
for y=1:size(m_img,1)
    for x=1:size(m_img,2)
        [m,v,p]=CollectImageStats(I(y_idx(y):y_idx(y+1)-1,x_idx(x):x_idx(x+1)-1),channels);
        m_img(y,x)=m;
        v_img(y,x)=v;
		p_img(y,x)=p;
    end
end

II=zeros(size(I));

y_interp=(y_idx(2:end)+y_idx(1:end-1))/2;
x_interp=(x_idx(2:end)+x_idx(1:end-1))/2;

aa=bwconncomp((I>imresize(m_img+5*sqrt(v_img),size(I),'bilinear')),4);
stars = struct('centroid',{},'cov',{},'photons',{});
for i=aa.PixelIdxList
    ii=cell2mat(i);
	%note: Matlab stores images in column major order
    Px=floor((ii-1)/size(I,1))+1;
    Py=mod(ii-1,size(I,1))+1;
    if (min(Px)<max(Px)&&min(Py)<max(Py))
		P=[Px Py];

		Px_clipped=min(max(Px,x_interp(1)),x_interp(end));
		Py_clipped=min(max(Py,y_interp(1)),y_interp(end));
		
		bg_val=interp2(x_interp,y_interp,m_img,Px_clipped,Py_clipped);
		photons2val=interp2(x_interp,y_interp,p_img,Px_clipped,Py_clipped);
		
		val=I(ii)-bg_val; r_val=[val val];
		totalval=sum(val);
		
		centroid     = sum(r_val.*P,1)/totalval; r_centroid=repmat(centroid,size(val));
		pixvar       = eye(2)*sum(val.^2)/(12*totalval^2);
		shotvar      = (P-r_centroid)'*diag(photons2val.*(val+bg_val))*(P-r_centroid)/totalval^2;
		cov = (pixvar+shotvar)^(1/2);
		photons      = sum(val./photons2val);
		stars(end+1) = struct('centroid',centroid,'cov',cov,'photons',photons);
		II(ii)=1;
    end
end
figure;imagesc(1./p_img)
%keyboard;
end

