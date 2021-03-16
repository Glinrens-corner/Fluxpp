#ifndef FLUXPP_STATE_HPP
#define FLUXPP_STATE_HPP
#include <tuple>
#include <vector>
#include <set>
#include <typeinfo>
#include <typeindex>
#include <mem_comparable_closure.hpp>
#include "app_event.hpp"


namespace fluxpp{
  using mem_comparable_closure::Function;
  using mem_comparable_closure::ClosureMaker;
  using widgets::AppEvent;
  namespace state{
    template <class T>
    class  StateSlice ;
    
    
    class BaseStateSlice{
    public:
      virtual std::pair<bool, std::vector<AppEvent>> dispatch_event(const AppEvent& event) = 0;
      virtual void extract(void*, std::type_index) = 0;
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
      
      void extract(void * obj, std::type_index type_idx ){
        if (type_idx == std::type_index(typeid(T) )){
	  new(obj) T{ this->state_};
	};
	return;
      };
      
      ~StateSlice()=default;
    public:
      reducer_t reducer_;
      T state_;
    };

    class StateImpl;

    class SynchronousStateInterface {
    public:
      SynchronousStateInterface():impl(nullptr){};
      SynchronousStateInterface(StateImpl* impl );
      SynchronousStateInterface(const SynchronousStateInterface& ) =delete;
      SynchronousStateInterface(SynchronousStateInterface&& other):impl(other.impl){
	other.impl=nullptr;
      };
      SynchronousStateInterface& operator=(SynchronousStateInterface& )=delete;
      SynchronousStateInterface& operator=(SynchronousStateInterface&& other){
	std::swap(other.impl, this->impl);
	return *this;
      };

      void get_state_slice_state(std::string slice, void* arg, std::type_index type_idx );

      std::set<std::string> get_updated_slices();
      
      ~SynchronousStateInterface();
    private:
      StateImpl* impl;
    };
    
    class State{
    public:
      State();
      State(State&& old):impl(old.impl){ };
      State(const State& old )= delete;
      State& operator=(State&& old) {
	std::swap(this->impl, old.impl);
	return *this;};
      State& operator=(const State&)=delete;
      ~State();
      SynchronousStateInterface get_synchronous_interface( ){
	return SynchronousStateInterface{this->impl};
      };
      
      void add_slice(std::string,
		     std::unique_ptr<BaseStateSlice>&& slice);
      void  dispatch_event( AppEvent  event);
    private:
      StateImpl * impl;
    };
  }; // state
};// fluxpp



#endif //FLUXPP_STATE_HPP
