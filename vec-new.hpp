#ifndef _OST_VEC_H_
#define _OST_VEC_H_
#include <tuple>
#include <cmath>
#include <string>
#include <algorithm>
#include <limits>

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
  decltype(std::max(arg,-arg)) {
    return std::max(arg,-arg);
  }
};

struct identity {
  template< class T >
  constexpr auto operator()( const T& arg) const ->
  decltype(arg) {
    return arg;
  }
};
struct rounding_error_variance {
  template< class T >
  constexpr auto operator()( const T& arg) const ->
  decltype(squared()(arg*std::numeric_limits<T>::epsilon())/12) {
    return squared()(arg*std::numeric_limits<T>::epsilon())/12;
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
  constexpr T operator()( const T& arg ) const {
    return arg/N;
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

//Functional operators
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
  template< typename T, typename U>
  constexpr auto operator()(const T &lhs, const U &rhs) const ->
  decltype(Operation()(lhs,rhs)) {
    return Operation()(lhs,rhs);
  }
  template<typename T, typename... U>
  constexpr auto operator()(const T &lhs, const std::tuple<U...> &rhs) const ->
  decltype(apply_right<sizeof...(U),Operation> ()(lhs,rhs)) {
    return apply_right<sizeof...(U),Operation> ()(lhs,rhs);
  }
  template<typename... T, typename U>
  constexpr auto operator()(const std::tuple<T...>&lhs, const U &rhs) const ->
  decltype(apply_left<sizeof...(T),Operation>()(lhs,rhs)) {
    return apply_left<sizeof...(T),Operation>()(lhs,rhs);
  }
  template<typename... T, typename... U>
  constexpr auto operator()(const std::tuple<T...>&lhs, const std::tuple<U...>&rhs) const ->
  decltype(apply<minimum()(sizeof...(T),sizeof...(U)),apply_all<Operation> >()(lhs,rhs)) {
    return apply<minimum()(sizeof...(T),sizeof...(U)),apply_all<Operation> >()(lhs,rhs);
  }
  template<typename... T>
  constexpr auto operator()(const std::tuple<T...>&arg) const ->
  decltype(apply<sizeof...(T),apply_all<Operation> >()(arg)) {
    return apply<sizeof...(T),apply_all<Operation> >()(arg);
  }
  template< typename T >
  constexpr auto operator()( const T& arg) const ->
  decltype(Operation()(arg)) {
    return Operation()(arg);
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
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(tuple_cat()(ld_cat<N-1>()(lhs,rhs),make_tuple()(tuple_cat()(get<N-2>()(lhs),get<N-1>()(rhs))))) {
    return tuple_cat()(ld_cat<N-1>()(lhs,rhs),make_tuple()(tuple_cat()(get<N-2>()(lhs),get<N-1>()(rhs))));
  }
};

template <>
struct ld_cat<1> {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(make_tuple()(get<0>()(rhs))) {
    return make_tuple()(get<0>()(rhs));
  }
};

template <std::size_t N>
struct du_cat {
  template<typename T1, typename T2>
  constexpr auto operator()(const T1 &lhs, const T2 &rhs) const ->
  decltype(tuple_cat()(apply<N-1,tuple_cat>()(lhs,rhs),make_tuple()(get<N-1>()(lhs)))) {
    return tuple_cat()(apply<N-1,tuple_cat>()(lhs,rhs),make_tuple()(get<N-1>()(lhs)));
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
struct cholesky_row3
{
  //Calculate and append middle column(s)
  template<typename T1,typename T2,typename T3>
  constexpr auto operator()(const T1 &arg, const T2 &left, const T3 &up) const ->
  decltype(tuple_cat()(left,make_tuple()((arg-dot<N-1>()(left,up))/get<N-1>()(up)))) {
    return tuple_cat()(left,make_tuple()((arg-dot<N-1>()(left,up))/get<N-1>()(up)));
  }
};
template <std::size_t N>
struct cholesky_row
{
  //Calculate and append final column
  template<typename T1,typename T2>
  constexpr auto operator()(const T1 &arg, const T2 &left) const ->
  decltype(tuple_cat()(left,make_tuple()(stable_sqrt()(arg-dot<N-1>()(left,left))))) {
    return tuple_cat()(left,make_tuple()(stable_sqrt()(arg-dot<N-1>()(left,left))));
  }
  //Return row N
  template<typename... T1,typename T2>
  constexpr auto operator()(const std::tuple<T1...>&arg, const T2 &up) const ->
  decltype(cholesky_row3<N>()(get<N-1>()(arg),cholesky_row<N-1>()(arg,up),get<N-1>()(up))) {
    return cholesky_row3<N>()(get<N-1>()(arg),cholesky_row<N-1>()(arg,up),get<N-1>()(up));
  }
};
template <>
struct cholesky_row<1>
{
  //Calculate first column
  template<typename... T1,typename T2>
  constexpr auto operator()(const std::tuple<T1...>&arg, const T2 &up) const ->
  decltype(make_tuple()(get<0>()(arg)/get<0>()(get<0>()(up)))) {
    return make_tuple()(get<0>()(arg)/get<0>()(get<0>()(up)));
  }
};
template <std::size_t N>
struct cholesky2
{
  //Calculate and append row N
  template<typename... T1,typename T2>
  constexpr auto operator()(const std::tuple<T1...>&arg, const T2 &up) const ->
  decltype(tuple_cat()(up,make_tuple()(cholesky_row<N>()(get<N-1>()(get<N-1>()(arg)),cholesky_row<N-1>()(get<N-1>()(arg),up))))) {
    return tuple_cat()(up,make_tuple()(cholesky_row<N>()(get<N-1>()(get<N-1>()(arg)),cholesky_row<N-1>()(get<N-1>()(arg),up))));
  }
};
template <std::size_t N>
struct cholesky
{
  //Return cholesky decomposition N
  template<typename... T1>
  constexpr auto operator()(const std::tuple<T1...>&arg) const ->
  decltype(cholesky2<N>()(arg,cholesky<N-1>()(arg))) {
    return cholesky2<N>()(arg,cholesky<N-1>()(arg));
  }
};

template <>
struct cholesky<1>
{
public:
  //Return first cholesky decomposition
  template<typename... T1>
  constexpr auto operator()(const std::tuple<T1...>&arg) const ->
  decltype(make_tuple()(make_tuple()(stable_sqrt()(get<0>()(get<0>()(arg)))))) {
    return make_tuple()(make_tuple()(stable_sqrt()(get<0>()(get<0>()(arg)))));
  }
};
//unscented transform - arg is tuple(mean, cholesky(covarience))
template <std::size_t N>
struct unscented { 
  template<typename T>
  constexpr auto operator()(const T &arg) const ->
  decltype(tuple_cat()(transpose<N,N>()(du_cat<N>()(apply_all<minus>()(get<0>()(arg),apply_all<sqrt_multiplies<N> >()(get<1>()(arg))),upper_triangular<N, identity>()(get<0>()(arg)))),make_tuple()(apply_all<sqrt_multiplies<1> >()(get<0>()(arg))),
                       transpose<N,N>()(du_cat<N>()(apply_all<plus >()(get<0>()(arg),apply_all<sqrt_multiplies<N> >()(get<1>()(arg))),upper_triangular<N, identity>()(get<0>()(arg)))))) {
    return tuple_cat()(transpose<N,N>()(du_cat<N>()(apply_all<minus>()(get<0>()(arg),apply_all<sqrt_multiplies<N> >()(get<1>()(arg))),upper_triangular<N, identity>()(get<0>()(arg)))),make_tuple()(apply_all<sqrt_multiplies<1> >()(get<0>()(arg))),
                       transpose<N,N>()(du_cat<N>()(apply_all<plus >()(get<0>()(arg),apply_all<sqrt_multiplies<N> >()(get<1>()(arg))),upper_triangular<N, identity>()(get<0>()(arg)))));
  }
};

template <std::size_t N1, std::size_t N2>
struct cov3
{
  template<typename T1,typename T2>
  constexpr auto operator()(const T1 &arg, const T2 &rounding_) const ->
  decltype(ld_cat<N2>()(apply<N2,const_divides<N1-1> >()(lower_triangular<N2,dot<N1> >()(arg,arg)),apply_all<make_tuple>()(apply<N2,plus>()(apply_all<const_divides<N1-1> >()(apply<N2,sum<N1> >()(apply_all<squared>()(arg))),rounding_)))) {
    return ld_cat<N2>()(apply_all<const_divides<N1-1> >()(lower_triangular<N2,dot<N1> >()(arg,arg)),apply_all<make_tuple>()(apply<N2,plus>()(apply_all<const_divides<N1-1> >()(apply<N2,sum<N1> >()(apply_all<squared>()(arg))),rounding_)));
  }
};
//calculate mean and covariance of a set of points, including rounding error
template <std::size_t N1, std::size_t N2>
struct cov2
{
  template<typename T1, typename T2, typename T3>
  constexpr auto operator()(const T1 &arg, const T2 &mean_, const T3 &rounding_) const ->
  decltype(make_tuple()(mean_,cov3<N1,N2>()(transpose<N1,N2>()(apply_left<N1,apply<N2,minus> >()(arg,mean_)),rounding_))) {
    return make_tuple()(mean_,cov3<N1,N2>()(transpose<N1,N2>()(apply_left<N1,apply<N2,minus> >()(arg,mean_)),rounding_));
  }
};
template <std::size_t N1, std::size_t N2>
struct cov
{
  template<typename T1>
  constexpr auto operator()(const T1 &arg) const ->
  decltype(cov2<N1,N2>()(arg,mean<N1,N2>()(arg),sum<N1,N2>()(apply_all<rounding_error_variance>()(arg)))) {
    return cov2<N1,N2>()(arg,mean<N1,N2>()(arg),sum<N1,N2>()(apply_all<rounding_error_variance>()(arg)));
  }
};
template <std::size_t N2>
struct cov<1,N2>
{
public:
  template<typename T1>
  constexpr auto operator()(const T1 &arg) const ->
  decltype(make_tuple()(get<0>()(arg),ld_cat<N2>()(lower_triangular<N2,identity>()(fill<N2>()(0)),transpose<1,N2>()(apply_all<rounding_error_variance>()(arg))))) {
    return make_tuple()(get<0>()(arg),ld_cat<N2>()(lower_triangular<N2,identity>()(fill<N2>()(0)),transpose<1,N2>()(apply_all<rounding_error_variance>()(arg))));
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
