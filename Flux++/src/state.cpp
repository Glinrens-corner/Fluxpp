#include <cassert>
#include "state.hpp"
#include <map>

namespace fluxpp{
  namespace state {
    // SubdivisibleStateSliceHolder:public BaseStateSlice
    class StateImpl {
    public:
      
      bool add_slice (std::string_view& loc, std::unique_ptr<BaseStateSlice>&& slice ){
	if(loc.find("/")!= std::string::npos  ){
	  // currently not prepared for this
	  throw std::exception();
	}
        const auto [first, success ] = this->slices_.insert({std::string {loc}, std::move(slice)} );
	return success;
      }

      
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
	std::set<std::string> new_set{};
	std::swap(this->updated_slices_, new_set);
	return new_set;
      };
      
      void lock(){};
      void unlock(){};
    private:
      std::set<std::string> updated_slices_{};
      std::map<std::string, std::unique_ptr<BaseStateSlice>> slices_;
    };
    SynchronousStateInterface::SynchronousStateInterface(StateImpl* impl):impl(impl){
      this->impl->lock();
    }
    SynchronousStateInterface::~SynchronousStateInterface(){
      if(this->impl) {
	this->impl->unlock();
      };
    };
    
    std::set<std::string> SynchronousStateInterface::get_updated_slices(){
      this->impl->get_updated_slices();
    };
    
    State::State():impl( new StateImpl{}){}
    
    State::~State(){ delete this->impl;};

    void State::add_slice(const std::string& loc,
			  std::unique_ptr<BaseStateSlice>&& slice){
      std::string_view view( loc);
      if((view.find("state/") != 0) ){
	throw std::exception();
      };
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
