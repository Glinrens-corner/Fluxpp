#include <doctest/doctest.h>
#include <type_traits>
#include "meta.hpp"





namespace {

  template<std::size_t n, class ...Ts>
  struct drop_n_variadic_core;

  template<std::size_t n, class first_t, class ...Ts>
  struct drop_n_variadic_core<n,first_t,Ts...>{
    static_assert(sizeof...(Ts) +1>= n);
    using type = typename drop_n_variadic_core<n-1, Ts...>::type;
  };

  template<class first_t, class ...Ts>
  struct drop_n_variadic_core<0,first_t,Ts... >{
    using type = fluxpp::meta::type_list<first_t,Ts...>;

  };
  template<>
  struct drop_n_variadic_core<0 >{
    using type = fluxpp::meta::type_list<>;

  };
  
  template<std::size_t n>
  struct drop_n_variadic{

    template<class ...T>
    using fn =  typename drop_n_variadic_core<n, T...>::type;
  };

  
  template<std::size_t n>
  struct drop_n{
    template<class list_t >
    using fn = typename list_t::template apply<drop_n_variadic<n>::template fn>;

  };

  template<class T>
  using to_int = int;
  
};

TEST_CASE("meta::map"){
  

}


TEST_CASE("meta::apply_to_t -> meta::map "){
  using namespace fluxpp::meta;
  using G = drop_n<2>::template fn<type_list<int, float, long>>;
  CHECK(std::is_same<G, type_list<long>>::value);
  CHECK(std::is_same<
          apply_to_t<type_list<int, float, long>,
          drop_n<2>::template fn
        >,
        type_list<long>
        >::value);


  CHECK(std::is_same<
        apply_to_t<type_list<int,float,long>,
        map<to_int>::template fn
        >,
        type_list<int,int,int>
        >::value);

  CHECK(std::is_same<
        apply_to_t<type_list<int, float, long>,
           drop_n<2>::template fn,
           map<to_int>::template fn
        >,
        type_list<int>
        >::value);
};
