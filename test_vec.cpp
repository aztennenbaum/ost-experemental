#include "vec.hpp"

#include <tuple>
#include <cassert>
#include <iostream>
#include <typeinfo>
#include <string>
#include <regex>

std::string to_matlab(std::string str) {
  return std::regex_replace(std::regex_replace(std::regex_replace( str, std::regex("\\),\\("), "];[" ),std::regex("\\("),"["),std::regex("\\)"),"]");
}
struct inv {
  template< class T>
  constexpr auto operator()( const T& arg) const ->
  decltype(1.0/arg) {
    return 1.0/arg;
  }
};
int main()
{
  auto a = make_tuple()(2,3,5, 7);
  auto b = make_tuple()(7,11,13);
  assert(make_tuple()(-16,9,1)==cross<3>()(a,b));
  assert((make_tuple()(  0,0,0)==cross<3,1>()(a,a)));
  assert(112==dot<3>()(b,a));
  
  assert((to_str()(a)=="(2,3,5,7)"));
  assert((reduce<3,logical_and>()(
    apply<3,greater>()(
      make_tuple()(0.001,0.001,0.001),
      apply<3,absolute>()(
        apply<3,minus>()(
          normalize<3>()(a),
          make_tuple()(0.324443,0.486664,0.811107)
        )
      )
    )
  )));
  
  const auto mat=make_tuple()(a,b,b);
  assert((to_str()(dot<3,3>() (mat,a))=="(38,112,112)"));
  assert((to_str()(dot<3,1,3>() (transpose<1,3>()(make_tuple()(a)),make_tuple()(b)))=="((14,22,26),(21,33,39),(35,55,65))"));
  assert((to_str()(transpose<1,3>()(make_tuple()(a)))=="((2),(3),(5))"));
  assert((to_str()(apply<1,get<2> > ()(make_tuple()(a)))=="(5)"));
  assert((to_str()(dot<3,3,3>()(mat,mat))=="((60,94,114),(182,285,347),(182,285,347))"));
  assert((to_str()(apply<1,get<2> > ()(make_tuple()(a)))=="(5)"));
  assert((to_str()(transpose<3,1>()(transpose<1,3>()(make_tuple()(a))))=="((2,3,5))"));
  auto c = lower_triangular<4,multiplies>()(a,a);
  assert((to_str()(c)=="((6),(10,15),(14,21,35))"));
  auto d = ld_cat<4>()(c,transpose<1,4>()(make_tuple()(a)));
  assert((to_str()(d)=="((2),(6,3),(10,15,5),(14,21,35,7))"));
  
  auto upper=make_tuple()(make_tuple()(1,2,3),make_tuple()(4,5),make_tuple()(6));

  assert((to_str() (du_cat<4>()(d,upper))=="((2,1,2,3),(6,3,4,5),(10,15,5,6),(14,21,35,7))"));
  
  auto e = lower_triangular<4,identity>()(a);
  assert((to_str()(e)=="((2),(2,3),(2,3,5))"));
  
  assert((to_str()(apply<3,identity,1>()(a)),"(3,5,7)"));
  
  auto f = upper_triangular<4,multiplies>()(a,a);
  assert((to_str()(f)=="((6,10,14),(15,21),(35))"));
  
  auto g = upper_triangular<4,identity>()(a);
  assert((to_str()(g)=="((2,2,2),(3,3),(5))"));

  auto h = make_tuple()(make_tuple()(25),make_tuple()(15,18),make_tuple()(-5, 0,11));
  auto i = cholesky<3>()(h);
  assert((to_str()(i)=="((5.000000),(3.000000,3.000000),(-1.000000,1.000000,3.000000))"));
  assert((to_str()(apply_all<sqrt_multiplies<2> >()(i))=="((7.071068),(4.242641,4.242641),(-1.414214,1.414214,4.242641))"));
  auto j = make_tuple()(b,i);
  auto k = unscented<3>()(j);
  assert((to_str()(k)=="((-1.660254,5.803848,14.732051),(7,5.803848,11.267949),(7,11,7.803848),(7.000000,11.000000,13.000000),(15.660254,16.196152,11.267949),(7,16.196152,14.732051),(7,11,18.196152))"));
  auto l1 = inv_unscented<1,3>()(make_tuple()(make_tuple()(0.995f,0.0995f,0.0f)));
  auto l2 = inv_unscented<7,3>()(unscented<3>()(l1));
  auto l3 = inv_unscented<7,3>()(unscented<3>()(l2));
  auto l4 = inv_unscented<7,3>()(unscented<3>()(l3));
  auto l5 = inv_unscented<7,3>()(unscented<3>()(l4));
  auto l6 = inv_unscented<7,3>()(unscented<3>()(l5));
  auto l7 = inv_unscented<7,3>()(unscented<3>()(l6));
  auto l8 = inv_unscented<7,3>()(unscented<3>()(l7));
  auto l9 = inv_unscented<7,3>()(unscented<3>()(l8));
  assert((to_str()(apply_all<inv>()(l9))=="((1.005025,10.050250,inf),((3450364.958615),(244734992.529621,42777618.631158),(inf,inf,9223372586610622464.000000)))"));
  
  std::cout<<to_str()(lower_triangular<3,identity>()(fill<3>()(0)))<<"\n";
  std::cout<<to_str()(apply_all<rounding_error_variance>()(get<0>()(make_tuple()(make_tuple()(0.995f,0.0995f,0.0f)))))<<"\n";
}

