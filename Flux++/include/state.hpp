#ifndef FLUXPP_STATE_HPP
#define FLUXPP_STATE_HPP
#include <tuple>
#include <vector>
#include <mem_comparable_closure.hpp>
#include "widget.hpp"


namespace fluxpp{
  using mem_comparable_closure::Function;
  using mem_comparable_closure::ClosureMaker;
  namespace state{
    template <class T>
    struct State{
    public:
      using reducer_t Function<std::pair<T, std::vector<AppEvent > >, T,AppEvent>;
    public:
      reducer_t reducer_;
      T state_;
    };

  };
};



#endif //FLUXPP_STATE_HPP
