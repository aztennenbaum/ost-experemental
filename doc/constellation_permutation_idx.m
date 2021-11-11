function [ idx ] = constellation_permutation_idx( in )
in1=in;
in2=constellation_permute(in,2*ones(size(in,1),1));
in3=constellation_permute(in,3*ones(size(in,1),1));
in4=constellation_permute(in,4*ones(size(in,1),1));
in5=constellation_permute(in,5*ones(size(in,1),1));
in6=constellation_permute(in,6*ones(size(in,1),1));

sensitivity1=constellation_normalized_sensitivity(in1);
sensitivity2=constellation_normalized_sensitivity(in2);
sensitivity3=constellation_normalized_sensitivity(in3);
sensitivity4=constellation_normalized_sensitivity(in4);
sensitivity5=constellation_normalized_sensitivity(in5);
sensitivity6=constellation_normalized_sensitivity(in6);

cr1=sum(crossratio_nocomplex(in1).^2,2);
cr2=sum(crossratio_nocomplex(in2).^2,2);
cr3=sum(crossratio_nocomplex(in3).^2,2);
cr4=sum(crossratio_nocomplex(in4).^2,2);
cr5=sum(crossratio_nocomplex(in5).^2,2);
cr6=sum(crossratio_nocomplex(in6).^2,2);

sensitivity=[sensitivity1,sensitivity2,sensitivity3,sensitivity4,sensitivity5,sensitivity6];
cr=[cr1,cr2,cr3,cr4,cr5,cr6];

%[ sensitivity_old , cr_old] = crossratio_jacobian_score_nocomplex2( in )

% [~,idx]=min([sensitivity1,sensitivity2,sensitivity3],[],2);
% idx=idx+3*(sum(crossratio_nocomplex(constellation_permute(in,idx)).^2,2)>1);
 [~,idx]=min(sensitivity.*cr,[],2);
% idx=3*(cr1>1)+1;

cr1=crossratio_nocomplex(in1);
cr2=crossratio_nocomplex(in2);
cr3=crossratio_nocomplex(in3);
cr4=crossratio_nocomplex(in4);
cr5=crossratio_nocomplex(in5);
cr6=crossratio_nocomplex(in6);

cr1=(cr1(:,1)*2-1).^2+cr1(:,2).^2;
cr2=(cr2(:,1)*2-1).^2+cr2(:,2).^2;
cr3=(cr3(:,1)*2-1).^2+cr3(:,2).^2;
cr4=(cr4(:,1)*2-1).^2+cr4(:,2).^2;
cr5=(cr5(:,1)*2-1).^2+cr5(:,2).^2;
cr6=(cr6(:,1)*2-1).^2+cr6(:,2).^2;
cr=[cr1,cr2,cr3,cr4,cr5,cr6];
[~,idx]=min(cr,[],2);
end

