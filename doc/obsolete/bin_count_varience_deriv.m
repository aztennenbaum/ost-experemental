clear all
syms bin_count_1 bin_count_2 bin_count_n
bin_count=[bin_count_1 bin_count_2 bin_count_n];
assume(bin_count,'positive')
[cf_x,cf_v]=propogate_gaussian(sum(bin_count),bin_count'/sum(bin_count),bin_count,diag(bin_count));
cf_x=simplify(cf_x)
cf_v=simplify(cf_v)
simplify(cf_v-(diag(bin_count)-bin_count'*bin_count/sum(bin_count)))
