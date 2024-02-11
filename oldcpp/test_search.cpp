#include "vec.hpp"
#include "search.hpp"
#include <vector>
#include <iostream>
#include <string>

int main() {
  std::vector<std::tuple<float,float,float> > test(100000);
  std::default_random_engine e1(500); 
  std::normal_distribution<float> norm_dist;
  for (auto it=test.begin(); it != test.cend(); ++it) {
    *it=normalize<3>()(std::make_tuple(norm_dist(e1),norm_dist(e1),norm_dist(e1)));
  }
  implicit_kdtree<lt<0>,10,lt<0>,lt<1>,lt<2> >::sort(test.begin(),test.end());
  search_results<std::tuple<float,float,float> > results(test.size());
  std::tuple<float,float,float> rand_vec, lower, upper;
  size_t results_count;
  for (int i=0;i<50000; i++) {
    results.reset();
    rand_vec=normalize<3>()(std::make_tuple(norm_dist(e1),norm_dist(e1),norm_dist(e1)));
    lower=apply<3,minus>()(rand_vec,std::make_tuple(0.05f,0.05f,0.05f));
    upper=apply<3,plus>()(rand_vec,std::make_tuple(0.05f,0.05f,0.05f));
    implicit_kdtree<lt<0>,10,lt<0>,lt<1>,lt<2> >::search(test.begin(),test.end(),results,lower,upper);
    results_count=0;
    for (auto it=results.cbegin(); it!=results.cend(); ++it) {
      if (reduce<3,logical_and>()(apply<3,less_equal   >()(*it,upper)) &&
          reduce<3,logical_and>()(apply<3,greater_equal>()(*it,lower))) results_count++;
    }
  }
  std::cout << "results_count=" << results_count << "\n";
  size_t test_count=0;
  for (auto it=test.cbegin(); it!=test.cend(); ++it) {
    if (reduce<3,logical_and>()(apply<3,less_equal   >()(*it,upper)) &&
        reduce<3,logical_and>()(apply<3,greater_equal>()(*it,lower))) test_count++;
  }
  
  std::cout << "test_count=" << test_count << "\n";
}
