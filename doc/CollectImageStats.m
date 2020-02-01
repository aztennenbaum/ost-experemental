function [ m, v] = CollectImageStats( I )
I=mean(I,3)/255;

[bin_count,bin_val]=imhist(I);
m_idx=find(cumsum(bin_count)>sum(bin_count)/2,1);
m=bin_val(m_idx);
v=sum(bin_count(1:m_idx).*((m-bin_val(1:m_idx)).^2))/sum(bin_count(1:m_idx));
m_img=zeros(ceil(size(I)/32));
v_img=zeros(size(m_img));
y_idx=round(linspace(1,size(I,1)+1,size(m_img,1)+1));
x_idx=round(linspace(1,size(I,2)+1,size(m_img,2)+1));
for y=1:size(m_img,1)
    for x=1:size(m_img,2)
        tile=I(y_idx(y):y_idx(y+1)-1,x_idx(x):x_idx(x+1)-1);
        m_img(y,x)=mean(tile(:));
        v_img(y,x)=var(tile(:));
    end
end

photons2val=mean(v_img(:)./m_img(:));
v_img=m_img*photons2val;
figure;imagesc(I);
II=zeros(size(I));
aa=bwconncomp((I>imresize(m_img+5*sqrt(v_img),size(I))),4);
bb=imresize(m_img,size(I));
for i=aa.PixelIdxList
    ii=cell2mat(i);
    Px=floor((ii-1)/size(I,1))+1;
    Py=mod(ii-1,size(I,1))+1;
    if (min(Px)<max(Px)&&min(Py)<max(Py))
       P=[Px Py];
       bg_val=bb(ii);
       val=I(ii)-bg_val; r_val=[val val];
       totalval=sum(val);
       centroid = sum(r_val.*P,1)/totalval; r_centroid=repmat(centroid,size(val));
           pixvar      = eye(2)*sum(val.^2)/(12*totalval^2);
           shotvar     = photons2val*(P-r_centroid)'*diag(val+bg_val)*(P-r_centroid)/totalval^2;
%           centroid
%           (pixvar+shotvar)^(1/2)
           II(ii)=1;
    end
end
figure;imagesc(II)
keyboard;
end

