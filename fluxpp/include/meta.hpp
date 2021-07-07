#ifndef FLUXPP_META_HPP
#define FLUXPP_META_HPP
#include <type_traits>

namespace fluxpp{
  namespace meta {
    template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
    template<class... Ts> overload(Ts...) -> overload<Ts...>;
    
  }// meta

  namespace meta{
    // use it as show<my_argument>
    // the errormessage shows the name of my_argument
    template<class T>
    struct show;

    // a general metatype for a list of types
    template <class ...Ts>
    struct type_list{
      template <template<class ...> typename V>
      using apply = V<Ts...>;
      
      static constexpr std::size_t size()noexcept{return sizeof...(Ts);}; 
    };

    
    namespace detail{
      template<template<class >typename function_T>
      struct map_variadic{
        template<class ...Ts>
        using fn = type_list<function_T<Ts>...>;

      };


    };

    template<template<class>typename function_T>
    struct map{

      template<class list_t>
      using fn = typename list_t::template apply<detail::map_variadic<function_T>::template fn>; 
    };
    

    template<class list_t,template<class >typename ... function_Ts>
    struct apply_to;
    
    template<class list_t, template <class>typename first_T,template<class >typename ... function_Ts>
    struct apply_to<list_t, first_T,function_Ts...>{
      using type = typename apply_to< first_T<list_t>,
                                     function_Ts...>::type;

    };

    template<class list_t>
    struct apply_to<list_t>{
      using type = list_t;
    };
    template<class list_t,template<class >typename ... function_Ts>
    using apply_to_t = typename apply_to<list_t ,function_Ts...>::type;
    
  } // meta
} // fluxpp









#endif //FLUXPP_META_HPP

