#ifndef FLUXPP_META_HPP
#define FLUXPP_META_HPP

namespace fluxpp{
  namespace meta {
    template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
    template<class... Ts> overload(Ts...) -> overload<Ts...>;
    
  }// meta

} // fluxpp









#endif //FLUXPP_META_HPP

