#include <cassert>
#include "state.hpp"
#include <map>

namespace fluxpp{
  namespace state {
    // SubdivisibleStateSliceHolder:public BaseStateSlice
    class State::StateImpl {
    public:
      
      bool add_slice (std::string_view& loc, std::unique_ptr<BaseStateSlice>&& slice ){
	if(loc.find("/")!= std::string::npos  ){
	  // currently not prepared for this
	  throw std::exception();
	}
        const auto [first, success ] = this->slices_.insert({std::string {loc}, std::move(slice)} );
	return success;
      }

      std::pair<bool,std::vector<AppEvent>> dispatch_event(std::string_view loc , const AppEvent& event ){
	if(loc.find("/")!= std::string::npos  ){
	  // currently not prepared for this
	  throw std::exception();
	}

	auto& slice = (*this->slices_.at(std::string{loc}));
	return slice.dispatch_event(event);
      }
      
    private:
      std::map<std::string, std::unique_ptr<BaseStateSlice>> slices_;
    };
    
    State::State():impl( new State::StateImpl{}){}
    
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
    
    std::pair<bool,std::vector<AppEvent>> State::dispatch_event(const AppEvent& event) {
      std::string_view view( event.target);
      if((view.find("state/") != 0) ){
	throw std::exception();
      };
      view.remove_prefix(std::string("state/").size());      
      return this->impl->dispatch_event(view,event);
    };
    
  } // state 
}// fluxpp
