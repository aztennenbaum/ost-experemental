#ifndef _OST_SEARCH_H_
#define _OST_SEARCH_H_

#include "search_results.hpp"
#include <algorithm>

template <std::size_t N>
struct lt { 
  template<typename T>
  bool operator()(const T &t1, const T &t2) {
    return std::get<N>(t1) < std::get<N>(t2);
  }
};

template <class CompareBucket>
struct binary_search {
  //Serial - bucket
  template <class RandomIt, typename T, template <typename> class S>
  static inline void search(RandomIt first, RandomIt last, search_results<T,S>& results, const T& lower, const T& upper) {
    if (std::distance(first,last)>0) {
      auto lb=std::lower_bound(first, last, lower, CompareBucket());
      auto ub=std::upper_bound(first, last, upper, CompareBucket());
      results.push_interval(std::make_pair(&*lb,&*ub));
    }
  }
  template <class RandomIt>
  static inline void sort(RandomIt first, RandomIt last) {
    if (std::distance(first,last)>0) {
			std::sort(first, last, CompareBucket());
    }
  }
};

template <class CompareBucket, int MaxDepth, class Compare1, class... CompareN>
struct implicit_kdtree {
  template <class RandomIt>
  static inline void sort(const RandomIt first, const RandomIt last) {
    if (std::distance(first,last)>0) {
      const auto mid = first+std::distance(first,last)/2;
      std::nth_element(first,mid,last,Compare1());
      implicit_kdtree<CompareBucket,MaxDepth-1,CompareN...,Compare1>::sort(first, mid);
      implicit_kdtree<CompareBucket,MaxDepth-1,CompareN...,Compare1>::sort(mid+1,last);
    }
  }
  template <class RandomIt, typename T, template <typename> class S>
  static inline void search(const RandomIt first, const RandomIt last, search_results<T,S>& results, const T& lower, const T& upper) {
    if (std::distance(first,last)>0) {
      auto mid = first+std::distance(first,last)/2;
      bool search_lower=(!Compare1()(*mid,lower));//*mid>=lower
      bool search_upper=(!Compare1()(upper,*mid));//*mid=<upper
      if (search_lower){
        implicit_kdtree<CompareBucket,MaxDepth-1,CompareN...,Compare1>::search(first,mid,results,lower,upper);
      }
      results.push_interval(std::make_pair(&*mid,&*mid+(search_lower && search_upper)));
      if (search_upper) {
        implicit_kdtree<CompareBucket,MaxDepth-1,CompareN...,Compare1>::search(mid+1,last,results,lower,upper);
      }
    }
  }
};
template <class CompareBucket, class Compare1, class... CompareN>
struct implicit_kdtree<CompareBucket,0,Compare1, CompareN...> : public binary_search<CompareBucket> {};

#endif
