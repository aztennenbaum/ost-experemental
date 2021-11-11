%close all
in=repelem(rand(800,8),100,1)+.001*randn(80000,8);
%plot(crossratio_nocomplex(in)*[1;1i],'.');
in=constellation_permute(in,constellation_permutation_idx( in ));
figure
plot(crossratio_nocomplex(in)*[2;1i]-1,'.');