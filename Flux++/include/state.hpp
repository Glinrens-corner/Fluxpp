#ifndef FLUXPP_STATE_HPP
#define FLUXPP_STATE_HPP
#include <tuple>
#include <vector>
#include <mem_comparable_closure.hpp>
#include "widget.hpp"


namespace fluxpp{
  using mem_comparable_closure::Function;
  using mem_comparable_closure::ClosureMaker;
  using widgets::AppEvent;
  namespace state{
    class BaseState{};
    
    template <class T>
    class  State: public BaseState{
    public:
      using reducer_t = Function<std::pair<T, std::vector<AppEvent > >, T,const AppEvent&>;
      template<class fn_t>
      State(T state, fn_t fn ):
	state_(state),
	reducer_(ClosureMaker<
		 std::pair<T, std::vector<AppEvent> >,
		 T,
		 const AppEvent&>::make(fn ).as_fun()){}; 
    public:
      reducer_t reducer_;
      T state_;
    };

  };
};



#endif //FLUXPP_STATE_HPP
