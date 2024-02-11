#ifndef _OST_VEC_H_
#define _OST_VEC_H_
#include <tuple>
#include <cmath>
#include <string>
#include <algorithm>
#include <limits>

//Requires gcc>=4.8.1 or clang >=3.8
//MSVC builds but is probably horribly slow due to lack of inlining 

//Arithmetic operations 
struct plus {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs + rhs) {
      return lhs + rhs;
    }
};
struct minus {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs - rhs) {
      return lhs - rhs;
    }
};
struct multiplies {
  template< class T, class U>
  constexpr auto operator()( const T& lhs, const U& rhs ) const ->
  decltype(lhs * rhs) {
    return lhs * rhs;
  }
};

struct divides {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs / rhs) {
      return lhs / rhs;
    }
};
struct modulus {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs % rhs) {
      return lhs % rhs;
    }
};
struct negate {
  template< class T >
    constexpr auto operator()( const T& arg) const ->
    decltype(-arg) {
      return -arg;
    }
};

struct squared {
  template< class T >
  constexpr auto operator()( const T& arg) const ->
  decltype(arg*arg) {
    return arg*arg;
  }
};

//Comparisons 
struct equal_to {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs == rhs) {
      return lhs == rhs;
    }
};
struct not_equal_to {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs != rhs) {
      return lhs != rhs;
    }
};
struct greater {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs > rhs) {
      return lhs > rhs;
    }
};
struct less {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs < rhs) {
      return lhs < rhs;
    }
};
struct greater_equal {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs >= rhs) {
      return lhs >= rhs;
    }
};
struct less_equal {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs <= rhs) {
      return lhs <= rhs;
    }
};

//Logical operations 
struct logical_and {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs && rhs) {
      return lhs && rhs;
    }
};
struct logical_or {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs || rhs) {
      return lhs || rhs;
    }
};
struct logical_not {
  template< class T >
    constexpr auto operator()( const T& arg) const ->
    decltype(!arg) {
      return !arg;
    }
};

//Bitwise operations 
struct bit_and {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs & rhs) {
      return lhs & rhs;
    }
};
struct bit_or {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs | rhs) {
      return lhs | rhs;
    }
};
struct bit_xor {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs ^ rhs) {
      return lhs ^ rhs;
    }
};
struct bit_not {
  template< class T >
    constexpr auto operator()( const T& arg) const ->
    decltype(~arg) {
      return ~arg;
    }
};

//Miscellaneous
struct minimum {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs>rhs?rhs:lhs) {
      return lhs>rhs?rhs:lhs;
    }
};
struct maximum {
  template< class T, class U>
    constexpr auto operator()( const T& lhs, const U& rhs ) const ->
    decltype(lhs>rhs?lhs:rhs) {
      return lhs>rhs?lhs:rhs;
    }
};
struct absolute {
  template< class T >
  constexpr auto operator()( const T& arg) const ->
  decltype(maximum()(arg,-arg)) {
    return maximum()(arg,-arg);
  }
};

struct identity {
  template< class T >
  constexpr auto operator()( const T& arg) const ->
  decltype(arg) {
    return arg;
  }
};
struct stable_sqrt {
  template< class T>
  constexpr auto operator()( const T& arg) const ->
  decltype(std::sqrt(arg)) {
    return std::sqrt(std::max(static_cast<decltype(std::sqrt(arg))>(arg),std::numeric_limits<decltype(std::sqrt(arg))>::min()));
  }
};

template <std::size_t N>
struct sqrt_multiplies {
  template< class T>
  constexpr auto operator()( const T& arg) const ->
  decltype(std::sqrt(arg)) {
    return std::sqrt(N) * arg;
  }
};


template <std::size_t N>
struct const_divides {
  template< class T>
  constexpr auto operator()( const T& arg) const ->
  decltype((T)(arg/N)) {
    return (T)(arg/N);
  }
};

//Tuple operators
struct make_tuple {
  template< class... T >
  constexpr auto operator()( const T&... args) const ->
  decltype(std::make_tuple(args...)) {
    return std::make_tuple(args...);
  }
};

struct tuple_cat {
  template< class... T>
    constexpr auto operator()( const T&... args) const ->
    decltype(std::tuple_cat(args...)) {
      return std::tuple_cat(args...);
    }
};

template <std::size_t N>
struct get {
  template< class T >
  constexpr auto operator()( const T& arg) const ->
  decltype(std::get<N>(arg)) {
    return std::get<N>(arg);
  }
};

template <std::size_t N>
struct fill {
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(tuple_cat()(fill<N-1>()(arg),make_tuple()(arg))) {
    return tuple_cat()(fill<N-1>()(arg),make_tuple()(arg));
  }
};

template <>
struct fill<1> {
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(make_tuple()(arg)) {
    return make_tuple()(arg);
  }
};


template<typename>
struct tuple_depth {
  static constexpr size_t value = 0;
};

template<typename T1, typename... T2>
struct tuple_depth<std::tuple<T1, T2...> > {
  static constexpr size_t value = tuple_depth<T1>::value+1;
};

//Functional operators
template <typename OpOuter, typename... OpInner> struct op_cat;
template <typename OpOuter, typename OpNext, typename... OpInner>
struct op_cat<OpOuter,OpNext,OpInner...> {
  template< class T >
  constexpr auto operator()( const T& arg) const ->
  decltype(OpOuter()(op_cat<OpNext,OpInner...>()(arg))) {
    return OpOuter()(op_cat<OpNext,OpInner...>()(arg));
  }
  template< class T , class U>
  constexpr auto operator()( const T& lhs, const U& rhs ) const ->
  decltype(OpOuter()(op_cat<OpNext,OpInner...>()(lhs),op_cat<OpNext,OpInner...>()(rhs))) {
    return OpOuter()(op_cat<OpNext,OpInner...>()(lhs),op_cat<OpNext,OpInner...>()(rhs));
  }
};

template <typename OpInner>
struct op_cat<OpInner> {
  template< class T >
  constexpr auto operator()( const T& arg) const ->
  decltype(OpInner()(arg)) {
    return OpInner()(arg);
  }
  template< class T , class U>
  constexpr auto operator()( const T& lhs, const U& rhs ) const ->
  decltype(OpInner()(lhs,rhs)) {
    return OpInner()(lhs,rhs);
  }
};

template <std::size_t N, class BinaryOperation, std::size_t Offset=0>
struct reduce {
  template<typename... T>
  constexpr auto operator()(const std::tuple<T...> &arg) const ->
  decltype(BinaryOperation()(reduce<N-1,BinaryOperation,Offset>()(arg),get<N+Offset-1>()(arg))) {
    return BinaryOperation()(reduce<N-1,BinaryOperation,Offset>()(arg),get<N+Offset-1>()(arg));
  }
};

template <class BinaryOperation, std::size_t Offset>
struct reduce<1,BinaryOperation, Offset> {
  template<typename... T>
  constexpr auto operator()(const std::tuple<T...> &arg) const ->
  decltype(get<Offset>()(arg)) {
    return get<Offset>()(arg);
  }
};
template <class BinaryOperation>
struct reduce_all {
  template<typename... T>
  constexpr auto operator()(const std::tuple<T...> &arg) const ->
  decltype(reduce<sizeof...(T),BinaryOperation>()(arg)) {
    return reduce<sizeof...(T),BinaryOperation>()(arg);
  }
};



template <std::size_t N, class Operation, std::size_t Offset=0>
struct apply {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(tuple_cat()(apply<N-1,Operation,Offset>()(lhs,rhs),make_tuple()(Operation()(get<N+Offset-1>()(lhs),get<N+Offset-1>()(rhs))))) {
    return tuple_cat()(apply<N-1,Operation,Offset>()(lhs,rhs),make_tuple()(Operation()(get<N+Offset-1>()(lhs),get<N+Offset-1>()(rhs))));
  }

  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(tuple_cat()(apply<N-1,Operation,Offset>()(arg),make_tuple()(Operation()(get<N+Offset-1>()(arg))))) {
    return tuple_cat()(apply<N-1,Operation,Offset>()(arg),make_tuple()(Operation()(get<N+Offset-1>()(arg))));
  }
};

template <class Operation, std::size_t Offset>
struct apply<1,Operation, Offset> {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(make_tuple()(Operation()(get<Offset>()(lhs),get<Offset>()(rhs)))) {
    return make_tuple()(Operation()(get<Offset>()(lhs),get<Offset>()(rhs)));
  }
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(make_tuple()(Operation()(get<Offset>()(arg)))) {
    return make_tuple()(Operation()(get<Offset>()(arg)));
  }
};
template <std::size_t N, class Operation, std::size_t Offset=0>
struct apply_right {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(tuple_cat()(apply_right<N-1,Operation,Offset>()(lhs,rhs),make_tuple()(Operation()(lhs,get<N+Offset-1>()(rhs))))) {
    return tuple_cat()(apply_right<N-1,Operation,Offset>()(lhs,rhs),make_tuple()(Operation()(lhs,get<N+Offset-1>()(rhs))));
  }
};

template <class Operation, std::size_t Offset>
struct apply_right<1,Operation, Offset> {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(make_tuple()(Operation()(lhs,get<Offset>()(rhs)))) {
    return make_tuple()(Operation()(lhs,get<Offset>()(rhs)));
  }
};

template <std::size_t N, class Operation, std::size_t Offset=0>
struct apply_left {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(tuple_cat()(apply_left<N-1,Operation,Offset>()(lhs,rhs),make_tuple()(Operation()(get<N+Offset-1>()(lhs),rhs)))) {
    return tuple_cat()(apply_left<N-1,Operation,Offset>()(lhs,rhs),make_tuple()(Operation()(get<N+Offset-1>()(lhs),rhs)));
  }
};

template <class Operation, std::size_t Offset>
struct apply_left<1,Operation, Offset> {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(make_tuple()(Operation()(get<Offset>()(lhs),rhs))) {
    return make_tuple()(Operation()(get<Offset>()(lhs),rhs));
  }
};

template <class Operation>
struct apply_all { 
  template< typename T1 >
  constexpr auto operator()( const T1& arg) const ->
  decltype(apply<std::tuple_size<T1>::value,Operation>()(arg)) {
    return apply<std::tuple_size<T1>::value,Operation>()(arg);
  }
  template< typename T1, typename T2 >
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(apply<minimum()(std::tuple_size<T1>::value,std::tuple_size<T2>::value),Operation>()(lhs,rhs)) {
    return apply<minimum()(std::tuple_size<T1>::value,std::tuple_size<T2>::value),Operation>()(lhs,rhs);
  }
};
template <class Operation>
struct apply_all_right { 
  template< typename T1, typename T2 >
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(apply_right<std::tuple_size<T2>::value,Operation>()(lhs,rhs)) {
    return apply_right<std::tuple_size<T2>::value,Operation>()(lhs,rhs);
  }
};
template <class Operation>
struct apply_all_left { 
  template< typename T1, typename T2 >
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(apply_left<std::tuple_size<T1>::value,Operation>()(lhs,rhs)) {
    return apply_left<std::tuple_size<T1>::value,Operation>()(lhs,rhs);
  }
};

//Triangular matrices
template <std::size_t N, class Operation>
struct lower_triangular {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(tuple_cat()(lower_triangular<N-1,Operation>()(lhs,rhs),make_tuple()(apply_left<N-1,Operation>()(lhs,get<N-1>()(rhs))))) {
    return tuple_cat()(lower_triangular<N-1,Operation>()(lhs,rhs),make_tuple()(apply_left<N-1,Operation>()(lhs,get<N-1>()(rhs))));
  }
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(tuple_cat()(lower_triangular<N-1,Operation>()(arg),make_tuple()(apply<N-1,Operation>()(arg)))) {
    return tuple_cat()(lower_triangular<N-1,Operation>()(arg),make_tuple()(apply<N-1,Operation>()(arg)));
  }
};

template <class Operation>
struct lower_triangular<2,Operation> {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(make_tuple()(make_tuple()(Operation()(get<1>()(lhs),get<0>()(rhs))))) {
    return make_tuple()(make_tuple()(Operation()(get<1>()(lhs),get<0>()(rhs))));
  }
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(make_tuple()(make_tuple()(Operation()(get<0>()(arg))))) {
    return make_tuple()(make_tuple()(Operation()(get<0>()(arg))));
  }
};

template <std::size_t N, class Operation, std::size_t Count=1>
struct upper_triangular {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(tuple_cat()(upper_triangular<N-1,Operation,Count+1>()(lhs,rhs),make_tuple()(apply_right<Count,Operation,N-1>()(get<N-2>()(lhs),rhs)))) {
    return tuple_cat()(upper_triangular<N-1,Operation,Count+1>()(lhs,rhs),make_tuple()(apply_right<Count,Operation,N-1>()(get<N-2>()(lhs),rhs)));
  }
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(tuple_cat()(upper_triangular<N-1,Operation,Count+1>()(arg),make_tuple()(fill<Count>()(Operation()(get<N-2>()(arg)))))) {
    return tuple_cat()(upper_triangular<N-1,Operation,Count+1>()(arg),make_tuple()(fill<Count>()(Operation()(get<N-2>()(arg)))));
  }
};

template <class Operation, std::size_t Count>
struct upper_triangular<2,Operation,Count> {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(make_tuple()(apply_right<Count,Operation,1>()(get<0>()(lhs),rhs))) {
    return make_tuple()(apply_right<Count,Operation,1>()(get<0>()(lhs),rhs));
  }
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(make_tuple()(fill<Count>()(Operation()(get<0>()(arg))))) {
    return make_tuple()(fill<Count>()(Operation()(get<0>()(arg))));
  }
};

template <std::size_t N>
struct ld_cat {
  template<typename T1, typename T2>
  constexpr auto ld_cat_column(const T1 &lhs, const T2 &rhs) const ->
  decltype(tuple_cat()(ld_cat<N-1>()(lhs,rhs),make_tuple()(tuple_cat()(get<N-2>()(lhs),get<N-1>()(rhs))))) {
    return tuple_cat()(ld_cat<N-1>()(lhs,rhs),make_tuple()(tuple_cat()(get<N-2>()(lhs),get<N-1>()(rhs))));
  }
  template<typename T1, typename... T2, typename... T3>
  constexpr auto operator()(const T1 &lhs, const std::tuple<std::tuple<T2...>,T3...> &rhs) const ->
  decltype(ld_cat_column(lhs,rhs)) {
    return ld_cat_column(lhs,rhs);
  }
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(ld_cat_column(lhs,apply_all<make_tuple>()(rhs))) {
    return ld_cat_column(lhs,apply_all<make_tuple>()(rhs));
  }
};

template <>
struct ld_cat<1> {
  template<typename T1, typename... T2, typename... T3>
  constexpr auto operator()(const T1 &lhs, const std::tuple<std::tuple<T2...>,T3...> &rhs) const ->
  decltype(make_tuple()(get<0>()(rhs))) {
    return make_tuple()(get<0>()(rhs));
  }
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(make_tuple()(make_tuple()(get<0>()(rhs)))) {
    return make_tuple()(make_tuple()(get<0>()(rhs)));
  }
};

template <std::size_t N>
struct du_cat {
  template<typename T1, typename T2>
  constexpr auto du_cat_column(const T1 &lhs, const T2 &rhs) const ->
  decltype(tuple_cat()(apply<N-1,tuple_cat>()(lhs,rhs),make_tuple()(get<N-1>()(lhs)))) {
    return tuple_cat()(apply<N-1,tuple_cat>()(lhs,rhs),make_tuple()(get<N-1>()(lhs)));
  }
  template<typename T1, typename... T2, typename... T3>
  constexpr auto operator()(const T1 &lhs, const std::tuple<std::tuple<T2...>,T3...> &rhs) const ->
  decltype(du_cat_column(lhs,rhs)) {
    return du_cat_column(lhs,rhs);
  }
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(du_cat_column(apply<N, make_tuple>()(lhs),rhs)) {
    return du_cat_column(apply<N, make_tuple>()(lhs),rhs);
  }
};

//Linear algebra
template <std::size_t N, std::size_t Offset=0>
struct cross { 
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const -> 
  decltype(make_tuple()(get<N+Offset-2>()(lhs)*get<N+Offset-1>()(rhs) - get<N+Offset-1>()(lhs)*get<N+Offset-2>()(rhs),
                        get<N+Offset-1>()(lhs)*get<N+Offset-3>()(rhs) - get<N+Offset-3>()(lhs)*get<N+Offset-1>()(rhs),
                        get<N+Offset-3>()(lhs)*get<N+Offset-2>()(rhs) - get<N+Offset-2>()(lhs)*get<N+Offset-3>()(rhs)))
  {
    return make_tuple()(get<N+Offset-2>()(lhs)*get<N+Offset-1>()(rhs) - get<N+Offset-1>()(lhs)*get<N+Offset-2>()(rhs),
                        get<N+Offset-1>()(lhs)*get<N+Offset-3>()(rhs) - get<N+Offset-3>()(lhs)*get<N+Offset-1>()(rhs),
                        get<N+Offset-3>()(lhs)*get<N+Offset-2>()(rhs) - get<N+Offset-2>()(lhs)*get<N+Offset-3>()(rhs));
  }
};
template <std::size_t N1, std::size_t... N> struct sum;
template <std::size_t N1>
struct sum<N1> { 
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(reduce<N1,plus>()(arg)) {
    return reduce<N1,plus>()(arg);
  }
};
template <std::size_t N1, std::size_t N2>
struct sum<N1,N2> { 
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(reduce<N1,apply<N2,plus> >()(arg)) {
    return reduce<N1,apply<N2,plus> >()(arg);
  }
};

template <std::size_t N1, std::size_t... N> struct mean;
template <std::size_t N1>
struct mean<N1> { 
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(sum<N1>()(arg)/N1) {
    return sum<N1>()(arg)/N1;
  }
};

template <std::size_t N1, std::size_t N2>
struct mean<N1,N2> { 
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(apply_left<N2,divides>()(sum<N1,N2>()(arg),N1)) {
    return apply_left<N2,divides>()(sum<N1,N2>()(arg),N1);
  }
};


template <std::size_t N1, std::size_t N2>
struct transpose { 
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(tuple_cat()(transpose<N1,N2-1>()(arg),make_tuple()(apply<N1,get<N2-1> > ()(arg)))) {
    return tuple_cat()(transpose<N1,N2-1>()(arg),make_tuple()(apply<N1,get<N2-1> > ()(arg)));
  }
};

template <std::size_t N1>
struct transpose<N1,1> {
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(make_tuple()(apply<N1,get<0> > ()(arg))) {
    return make_tuple()(apply<N1,get<0> > ()(arg));
  }
};

template <std::size_t N1, std::size_t... N> struct dot;
template <std::size_t N1, std::size_t N2>
struct dot<N1,N2,1> { 
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(apply<N1,make_tuple>()(dot<N1,N2>()(lhs,apply<N2,get<0> > ()(rhs)))) {
    return apply<N1,make_tuple>()(dot<N1,N2>()(lhs,apply<N2,get<0> > ()(rhs)));
  }
};
template <std::size_t N1, std::size_t N2, std::size_t N3>
struct dot<N1,N2,N3> { 
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(apply<N1,tuple_cat>()(dot<N1,N2,N3-1>()(lhs,rhs),apply<N1,make_tuple>()(dot<N1,N2>()(lhs,apply<N2,get<N3-1> > ()(rhs))))) {
    return apply<N1,tuple_cat>()(dot<N1,N2,N3-1>()(lhs,rhs),apply<N1,make_tuple>()(dot<N1,N2>()(lhs,apply<N2,get<N3-1> > ()(rhs))));
  }
};


template <std::size_t N1, std::size_t N2>
struct dot<N1,N2> { 
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(apply_left<N1,dot<N2> > ()(lhs,rhs)) {
    return apply_left<N1,dot<N2> > ()(lhs,rhs);
  }
};
template <std::size_t N>
struct dot<N> { 
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(sum<N>()(apply<N,multiplies>()(lhs,rhs))) {
    return sum<N>()(apply<N,multiplies>()(lhs,rhs));
  }
};
template <>
struct dot<1> { 
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(get<0>()(lhs)*get<0>()(rhs)) {
    return get<0>()(lhs)*get<0>()(rhs);
  }
};
template <std::size_t N>
struct norm { 
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(stable_sqrt()(dot<N>()(arg,arg))) {
    return stable_sqrt()(dot<N>()(arg,arg));
  }
};
template <std::size_t N>
struct normalize { 
  template<typename T>
  constexpr auto operator()(const T &arg) const -> 
  decltype(apply_left<N,divides>()(arg,norm<N>()(arg))) {
    return apply_left<N,divides>()(arg,norm<N>()(arg));
  }
};

template <std::size_t N>
class cholesky
{
  friend class cholesky<N+1>;
  //Calculate and append next element
  template<typename T1,typename T2,typename T3>
  static constexpr auto append_elem(const T1 &arg, const T2 &left, const T3 &up) ->
  decltype(tuple_cat()(left,make_tuple()((arg-dot<N-1>()(left,up))/get<N-1>()(up)))) {
    return tuple_cat()(left,make_tuple()((arg-dot<N-1>()(left,up))/get<N-1>()(up)));
  }
  //Return row N
  template<typename... T1,typename T2>
  static constexpr auto build_row(const std::tuple<T1...>&arg, const T2 &up) ->
  decltype(append_elem(get<N-1>()(arg),cholesky<N-1>::build_row(arg,up),get<N-1>()(up))) {
    return append_elem(get<N-1>()(arg),cholesky<N-1>::build_row(arg,up),get<N-1>()(up));
  }
  //Calculate and append diagonal element
  template<typename T1,typename T2>
  constexpr auto append_diag_elem(const T1 &arg, const T2 &left) const ->
  decltype(tuple_cat()(left,make_tuple()(stable_sqrt()(arg-dot<N-1>()(left,left))))) {
    return tuple_cat()(left,make_tuple()(stable_sqrt()(arg-dot<N-1>()(left,left))));
  }
  //Calculate and append row
  template<typename... T1,typename T2>
  constexpr auto append_row(const std::tuple<T1...>&arg, const T2 &up) const ->
  decltype(tuple_cat()(up,make_tuple()(append_diag_elem(get<N-1>()(get<N-1>()(arg)),cholesky<N-1>::build_row(get<N-1>()(arg),up))))) {
    return tuple_cat()(up,make_tuple()(append_diag_elem(get<N-1>()(get<N-1>()(arg)),cholesky<N-1>::build_row(get<N-1>()(arg),up))));
  }
public:
  //Return cholesky decomposition N
  template<typename... T1>
  constexpr auto operator()(const std::tuple<T1...>&arg) const ->
  decltype(append_row(arg,cholesky<N-1>()(arg))) {
    return append_row(arg,cholesky<N-1>()(arg));
  }
};

template <>
class cholesky<1>
{
  friend class cholesky<2>;
  //Calculate first element of row
  template<typename... T1,typename T2>
  static constexpr auto build_row(const std::tuple<T1...>&arg, const T2 &up) ->
  decltype(make_tuple()(get<0>()(arg)/get<0>()(get<0>()(up)))) {
    return make_tuple()(get<0>()(arg)/get<0>()(get<0>()(up)));
  }
public:
  //Return first cholesky decomposition
  template<typename... T1>
  constexpr auto operator()(const std::tuple<T1...>&arg) const ->
  decltype(make_tuple()(make_tuple()(stable_sqrt()(get<0>()(get<0>()(arg)))))) {
    return make_tuple()(make_tuple()(stable_sqrt()(get<0>()(get<0>()(arg)))));
  }
};

template <std::size_t N>
struct uncorrelated_lt
{
  template<typename T1>
  constexpr auto operator()(const T1 &arg) const ->
  decltype(ld_cat<N>()(lower_triangular<N,identity>()(fill<N-1>()(0)),arg)) {
    return ld_cat<N>()(lower_triangular<N,identity>()(fill<N-1>()(0)),arg);
  }  
};

//unscented transform - arg is tuple(mean, cholesky(covarience))
template<class Operation, std::size_t N, std::size_t D> struct calculate_sigma_points_lt;
template<class Operation, std::size_t N>
struct calculate_sigma_points_lt<Operation,N,2> {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(apply_all<apply_all_right<Operation> >()(lhs,apply<N,apply_all<sqrt_multiplies<N> > >()(rhs))) {
    return apply_all<apply_all_right<Operation> >()(lhs,apply<N,apply_all<sqrt_multiplies<N> > >()(rhs));
  }
};
template<class Operation, std::size_t N>
struct calculate_sigma_points_lt<Operation,N,1> {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(calculate_sigma_points_lt<Operation,N,2>()(lhs,uncorrelated_lt<N>()(rhs))) {
    return calculate_sigma_points_lt<Operation,N,2>()(lhs,uncorrelated_lt<N>()(rhs));
  }
};
template<class Operation, std::size_t N>
struct calculate_sigma_points_lt<Operation,N,0> {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(calculate_sigma_points_lt<Operation,N,1>()(lhs,fill<N>()(rhs))) {
    return calculate_sigma_points_lt<Operation,N,1>()(lhs,fill<N>()(rhs));
  }
};

//unscented transform - arg is tuple(mean, cholesky(covarience))
template <std::size_t N>
struct unscented { 
  template<class Operation, typename T1, typename T2>
  constexpr auto calculate_sigma_points(const T1 &lhs, const T2 &rhs) const ->
  decltype(transpose<N,N>()(du_cat<N>()(calculate_sigma_points_lt<Operation,N,tuple_depth<T2>::value>()(lhs,rhs),upper_triangular<N, identity>()(lhs)))) {
    return transpose<N,N>()(du_cat<N>()(calculate_sigma_points_lt<Operation,N,tuple_depth<T2>::value>()(lhs,rhs),upper_triangular<N, identity>()(lhs)));
  }
  
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(tuple_cat()(calculate_sigma_points<minus>(get<0>()(arg),get<1>()(arg)),make_tuple()(get<0>()(arg)),calculate_sigma_points<plus >(get<0>()(arg),get<1>()(arg)))) {
    return tuple_cat()(calculate_sigma_points<minus>(get<0>()(arg),get<1>()(arg)),make_tuple()(get<0>()(arg)),calculate_sigma_points<plus >(get<0>()(arg),get<1>()(arg)));
  }
};
struct rounding_error_variance {
  template< class T >
  constexpr auto operator()( const T& arg) const ->
  decltype(squared()(arg*std::numeric_limits<T>::epsilon())/12) {
    return squared()(arg*std::numeric_limits<T>::epsilon())/12;
  }
};
template <std::size_t N1, std::size_t N2>
struct worst_case_rounding_variance
{
  template<typename T1>
  constexpr auto operator()(const T1 &arg) const ->
  decltype(sum<N1,N2>()(apply<N1,apply<N2,rounding_error_variance> >()(arg))) {
    return sum<N1,N2>()(apply<N1,apply<N2,rounding_error_variance> >()(arg));
  }  
};

template <std::size_t N1, std::size_t N2>
class cov
{
  template<typename T1>
  constexpr auto calculate_variance(const T1 &arg_minus_mean) const ->
  decltype(apply<N2,const_divides<N1-1> >()(sum<N1,N2>()(apply<N1,apply<N2,squared> >()(arg_minus_mean)))) {
    return apply<N2,const_divides<N1-1> >()(sum<N1,N2>()(apply<N1,apply<N2,squared> >()(arg_minus_mean)));
  }
  template<typename T1>
  constexpr auto calculate_covariance_lt(const T1 &arg_minus_mean) const ->
  decltype(apply<N2-1,apply_all<const_divides<N1-1> > >()(lower_triangular<N2,dot<N1> >()(arg_minus_mean,arg_minus_mean))) {
    return apply<N2-1,apply_all<const_divides<N1-1> > >()(lower_triangular<N2,dot<N1> >()(arg_minus_mean,arg_minus_mean));
  }
  template<typename T1, typename T2>
  constexpr auto calculate_covariance(const T1 &arg, const T2 &arg_minus_mean) const ->
  decltype(ld_cat<N2>()(calculate_covariance_lt(transpose<N1,N2>()(arg_minus_mean)),apply<N2,plus>()(calculate_variance(arg_minus_mean),worst_case_rounding_variance<N1,N2>()(arg)))) {
    return ld_cat<N2>()(calculate_covariance_lt(transpose<N1,N2>()(arg_minus_mean)),apply<N2,plus>()(calculate_variance(arg_minus_mean),worst_case_rounding_variance<N1,N2>()(arg)));
  }  
  template<typename T1, typename T2>
  constexpr auto append_mean(const T1 &arg, const T2 &mean_) const ->
  decltype(make_tuple()(mean_,calculate_covariance(arg,apply_left<N1,apply<N2,minus> >()(arg,mean_)))) {
    return make_tuple()(mean_,calculate_covariance(arg,apply_left<N1,apply<N2,minus> >()(arg,mean_)));
  }
public:
  template<typename T1>
  constexpr auto operator()(const T1 &arg) const ->
  decltype(append_mean(arg,mean<N1,N2>()(arg))) {
    return append_mean(arg,mean<N1,N2>()(arg));
  }
};
template <std::size_t N2>
struct cov<1,N2>
{
public:
  template<typename T1>
  constexpr auto operator()(const T1 &arg) const ->
  decltype(make_tuple()(get<0>()(arg),uncorrelated_lt<N2>()(worst_case_rounding_variance<1,N2>()(arg)))) {
    return make_tuple()(get<0>()(arg),uncorrelated_lt<N2>()(worst_case_rounding_variance<1,N2>()(arg)));
  }
};
//apply to tuple(mean,cov)
template <std::size_t N1, std::size_t... N> struct inv_unscented;
template <std::size_t N1>
struct inv_unscented<N1>
{
  template<typename T1>
  constexpr auto operator()(const T1 &cov_) const ->
  decltype(make_tuple()(get<0>()(cov_),cholesky<N1>()(get<1>()(cov_)))) {
    return make_tuple()(get<0>()(cov_),cholesky<N1>()(get<1>()(cov_)));
  }
};

//apply to set of points
template <std::size_t N1, std::size_t N2>
struct inv_unscented<N1,N2>
{
  template<typename T1>
  constexpr auto operator()(const T1 &arg) const ->
  decltype(inv_unscented<N2>()(cov<N1,N2>()(arg))) {
    return inv_unscented<N2>()(cov<N1,N2>()(arg));
  }
};

//divide by lower triangular
template <std::size_t N>
class divided_by_lt
{
template<typename T1,typename T2,typename T3>
constexpr auto reduce_off_diag(const T1 &x,const T2 &m2, const T3 &m3) const ->
  decltype(apply<N-1,minus>()(x,apply_left<N-1,multiplies>()(m2,m3))) {
    return apply<N-1,minus>()(x,apply_left<N-1,multiplies>()(m2,m3));
  }
template<typename T1,typename T2,typename T3>
constexpr auto reduce_row(const T1 &x,const T2 &L,const T3 &diag_elem) const ->
  decltype(tuple_cat()(divided_by_lt<N-1>()(reduce_off_diag(x,get<N-1>()(L),diag_elem),L),make_tuple()(diag_elem))) {
    return tuple_cat()(divided_by_lt<N-1>()(reduce_off_diag(x,get<N-1>()(L),diag_elem),L),make_tuple()(diag_elem));
  }
public:
  template<typename T1,typename T2>
  constexpr auto operator()(const T1 &x, const T2 &L) const ->
  decltype(reduce_row(x,L,divides()(get<N-1>()(x),get<N-1>()(get<N-1>()(L))))) {
    return reduce_row(x,L,divides()(get<N-1>()(x),get<N-1>()(get<N-1>()(L))));
  }
};
template <>
class divided_by_lt<1>
{
public:
  template<typename T1,typename T2>
  constexpr auto operator()(const T1 &x, const T2 &L) const ->
  decltype(make_tuple()(divides()(get<0>()(x),get<0>()(get<0>()(L))))) {
    return make_tuple()(divides()(get<0>()(x),get<0>()(get<0>()(L))));
  }
};
//String conversion
struct to_str { 
  template< typename T >
  const std::string operator()( const T& arg) const {
    return std::to_string(arg);
  }
  template<typename T1, typename T2, typename...T>
  const std::string operator()(const std::tuple<T1, T2, T...>&arg) {
    const std::size_t N=sizeof...(T)+2;
    return std::string("(")+reduce<N-1,plus>()(apply_left<N-1,plus>()(apply<N-1,to_str>()(arg),","))+to_str()(get<N-1>()(arg)) +")";
  }
  template<typename T>
  const std::string operator()(const std::tuple<T>&arg) {
    return std::string("(")+to_str()(get<0>()(arg)) +")";
  }
};

#endif
