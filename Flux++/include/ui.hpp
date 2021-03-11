#ifndef FLUXPP_UI_HPP
#define FLUXPP_UI_HPP
#include <memory>
#include <queue>
#include "state.hpp"
#include "backend/base_backend.hpp"
#include "widget.hpp"
#include "render_tree.hpp"
// The Ui consists of three classes the render-tree, the backend and the State.
// the render tree wraps the user provided widget tree at initialization only the application widget is in the render tree.


namespace fluxpp{
  class Ui{
  public:
    template<class ...Ts>
    Ui(
       backend::BaseBackend* backend,
       widgets::application::Application<Ts...> application)
      :backend_(backend)
      ,queue_system_(std::make_unique<std::queue<AppEvent>>())
      ,state_(std::make_unique<state::State>())
      {
	this->render_tree_=
	  std::unique_ptr<RenderTree>(
	      new RenderTree(
		  std::unique_ptr<widgets::BaseWidget>(
		      new widgets::application::Application{std::move(application)}),
		  queue_system_.get(),
		  this->backend_,
		  this->state_.get()));
    };
    
    template<class T>
    void add_state_slice(
	const std::string& position,
	state::StateSlice<T> slice){
      this
	->state_
	->add_slice(
	  position,
	  std::unique_ptr<state::BaseStateSlice>(
	      new state::StateSlice<T>(slice)
	  )
	);
    };
    
  private:
    std::unique_ptr<std::queue<AppEvent>> queue_system_;
    std::unique_ptr<state::State> state_;
    std::unique_ptr<RenderTree> render_tree_{};
    backend::BaseBackend* backend_; 
  };
};















#endif // FLUXPP_UI_HPP
