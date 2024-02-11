close all
oc=(rand(250,8)-.5); %original constelations

N=50;
noise=.001*rand(size(oc,1)*N,8);
f_noise=0.1*(rand(size(oc,1)*N,1)-.5);

%permute then add noise
idx=constellation_permutation_idx( oc );
in=repelem(constellation_permute(oc,idx),N,1);
in=stereoproj_nocomplex(stereoprojinv_nocomplex( in, 1, 1)+noise,1,1);

plot(crossratio_nocomplex(in)*[2;1i]-1,'.');
%hold on;
%in=stereoproj_nocomplex(stereoprojinv_nocomplex( in, 1, 1),1+f_noise,1);
%plot(crossratio_nocomplex(in)*[2;1i]-1,'.');

%add noise then permute
% in=repelem(oc,N,1)+noise;
% idx=constellation_permutation_idx( in );
% unique(idx)
% in=constellation_permute(in,idx);
% hold on;
% plot(crossratio_nocomplex(in)*[2;1i]-1,'.');
