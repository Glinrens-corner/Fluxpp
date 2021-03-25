#include <cassert>
#include "state.hpp"
#include <queue>
#include "app_event.hpp"
#include <map>
#include <iostream>

namespace fluxpp{
  namespace state {
    using queue_t = std::queue<widgets::AppEvent>;
    
    // SubdivisibleStateSliceHolder:public BaseStateSlice
    class StateImpl {
    public:
      StateImpl(queue_t* queue):queue_(queue){};
    public:
      bool add_slice (std::string_view& loc, std::unique_ptr<BaseStateSlice>&& slice ){
	if(loc.find("/")!= std::string::npos  ){
	  // currently not prepared for this
	  throw std::exception();
	}
	
        const auto [first, success ] = this->slices_.insert({std::string {loc}, std::move(slice)} );
	return success;
      }

      void get_state_slice_state(
	std::string slice_path,
	void* arg,
	std::type_index type_idx ){
	
	std::string_view view( slice_path);
	if((view.find("state/") != 0) ){
	  throw std::exception();
	};
	view.remove_prefix(std::string("state/").size());      
	
	if(view.find("/")!= std::string::npos  ){
	  // currently not prepared for multilevel state
	  throw std::exception();
	};

    
	auto& slice = (*this->slices_.at(std::string{view}));
	slice.extract( arg,type_idx );
	return;
      };
      
      void add_updated_slice(std::string loc){
	this->updated_slices_.insert( std::move(loc));
      };

      void dispatch_events(){
	std::cout<< "dispatching event"<< std::endl;
	while(!this->queue_->empty()){
	  this->dispatch_event(this->queue_->front());
	  this->queue_->pop();
	};
      };
      
      std::vector<AppEvent> dispatch_event( AppEvent event ){
	std::string_view view( event.target);
	if((view.find("state/") != 0) ){
	  throw std::exception();
	};
	view.remove_prefix(std::string("state/").size());      
	
	if(view.find("/")!= std::string::npos  ){
	  // currently not prepared for multilevel state
	  throw std::exception();
	};

	auto& slice = (*this->slices_.at(std::string{view}));
	auto  [updated, follow_events] = slice.dispatch_event(event);
	this->updated_slices_.insert( std::move(event.target));
	return follow_events;
      }
      
      std::set<std::string> get_updated_slices(){
	this->dispatch_events();
	return std::set<std::string>(this->updated_slices_);
      };
      
      void lock(){};
      void unlock(){};
    private:
      queue_t* queue_;
      std::set<std::string> updated_slices_{};
      std::map<std::string, std::unique_ptr<BaseStateSlice>> slices_;
    };

    void SynchronousStateInterface::get_state_slice_state(
	std::string slice,
	void* arg,
	std::type_index type_idx ){
	this->impl->get_state_slice_state(slice, arg, type_idx);
	return;
      };      

    SynchronousStateInterface::SynchronousStateInterface(StateImpl* impl):impl(impl){
      this->impl->lock();
    };
    SynchronousStateInterface::~SynchronousStateInterface(){
      if(this->impl) {
	this->impl->unlock();
      };
    };
    
    std::set<std::string> SynchronousStateInterface::get_updated_slices(){
      return this->impl->get_updated_slices();
    };
    
    State::State(queue_t* queue):impl( new StateImpl{queue}){}
    
    State::~State(){ delete this->impl;};

    void State::add_slice(std::string loc,
			  std::unique_ptr<BaseStateSlice>&& slice){
      std::string_view view( loc);
      if((view.find("state/") != 0) ){
	throw std::exception();
      };
      this->impl->add_updated_slice(loc);
      view.remove_prefix(std::string("state/").size());
      if(! this->impl->add_slice(view,std::move(slice))){
	// insertion failed.
	throw std::exception();
      };
    };
    
    void State::dispatch_event( AppEvent event) {
      auto follow_events  = this->impl->dispatch_event(std::move(event));
      
    };
    
  } // state 
}// fluxpp
