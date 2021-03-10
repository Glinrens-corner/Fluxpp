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

    class BaseStateSlice{
    public:
      virtual std::pair<bool, std::vector<AppEvent>> dispatch_event(const AppEvent& event) = 0;
      virtual ~BaseStateSlice()=default;
    };
    
    
    template <class T>
    class  StateSlice final: public BaseStateSlice{
    public:
      using reducer_t = Function<
      std::pair<T, std::vector<AppEvent > >, T,const AppEvent&>;
      template<class fn_t>
      StateSlice(T state, fn_t fn ):
    	state_(state),
    	reducer_(ClosureMaker<
    		 std::pair<T, std::vector<AppEvent> >,
    		 T,
    		 const AppEvent&>::make(fn ).as_fun()){};
      std::pair<bool,std::vector<AppEvent>> dispatch_event(const AppEvent& event)  {
	auto [ new_state,vec] = this->reducer_(this->state_, event);
	this->state_ = std::move(new_state);
	return std::make_pair(true, vec);
      };
      ~StateSlice()=default;
    public:
      reducer_t reducer_;
      T state_;
    };

    class State{

    public:
      State();
      State(State&& old):impl(old.impl){ };
      State(const State& old )= delete;
      State& operator=(State&& old) {
	this->impl=old.impl;
	old.impl=nullptr;
	return *this;};
      State& operator=(const State&)=delete;
      ~State();
      void add_slice(const std::string&,
		     std::unique_ptr<BaseStateSlice>&& slice);
      std::pair<bool,std::vector<AppEvent>> dispatch_event(const AppEvent&  event);
    private:
      class StateImpl;
      StateImpl * impl;
    };
  }; // state
};// fluxpp



#endif //FLUXPP_STATE_HPP
