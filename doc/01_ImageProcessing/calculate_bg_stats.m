function [ bg_mean, bg_var ] = calculate_bg_stats( bin_count, bin_val, threshold_sigma)
    %UNTITLED3 Summary of this function goes here
    %   Detailed explanation goes here
    step = bin_val(2)-bin_val(1);
    bin_val=reshape(bin_val(bin_count>0),1,[]);
    bin_count=reshape(bin_count(bin_count>0),1,[]);
    c0 = double(bin_count);
    c1 = c0.*bin_val;
    c2 = c0.*bin_val.^2;

    s=zeros(size(bin_val));
    m=zeros(size(bin_val));
    v=zeros(size(bin_val));
    t=zeros(size(bin_val));

    for i=1:numel(bin_val)
        if i>1&&bin_val(i-1)<t(i-1)
            c0(i)=c0(i)+c0(i-1);
            c1(i)=c1(i)+c1(i-1);
            c2(i)=c2(i)+c2(i-1);
        end
        s(i) = c0(i);
        m(i) = c1(i)/c0(i);
        v(i) = c2(i)/c0(i) - m(i)*m(i) + step^2/12;
        m(i) = m(i) + step/2;
        t(i) = m(i) + sqrt(v(i))*threshold_sigma;
    end
    [~,idx]=max(c0);
    bg_mean=m(idx);%mean
    bg_var =v(idx);%varience
end

