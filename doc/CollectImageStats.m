function [ m, v, photons2val ] = CollectImageStats( I, channels)
	[bin_count,bin_val]=imhist(I,256*channels);
	old_idx=-1;
	idx=numel(bin_count);
	%numiter=0;
	while idx ~= old_idx
		m=sum(bin_val(1:idx).*bin_count(1:idx))/sum(bin_count(1:idx));
		v=sum((bin_val(1:idx)-m).^2.*bin_count(1:idx))/(sum(bin_count(1:idx))-1);
		old_idx=idx;
		idx=find(bin_val>(m+5*sqrt(v)),1)-1;
		%numiter=numiter+1;
	end
	%numiter
	%bias assuming uniform photon distribution within bins
	step=(bin_val(2)-bin_val(1));
	photons2val=(v-step^2/12)/(m+step/2);
end
