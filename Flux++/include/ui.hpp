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
  
    Ui(std::unique_ptr<std::queue<AppEvent>> queue_system,
       backend::BaseBackend* backend,
       std::unique_ptr<state::State> state,
       std::unique_ptr<RenderTree> render_tree)
      :backend_(backend)
      ,queue_system_(std::move(queue_system))
      ,state_(std::move(state))
      ,render_tree_(std::move(render_tree))
      {
      };
  public:
    
    template<class ...Ts>
    static Ui create(backend::BaseBackend* backend,
		     widgets::application::Application<Ts...> application
    ){
      auto queue_system = std::make_unique<std::queue<AppEvent>>();
      auto state = std::make_unique<state::State>();
      auto render_tree = std::make_unique<RenderTree>(
	  std::unique_ptr<widgets::application::ApplicationBase>(
	      new widgets::application::Application{std::move(application)}),
	  queue_system.get(),
	  backend,
	  state.get());
	  
      return Ui(
         std::move(queue_system),
	 backend,
	 std::move(state), 
	 std::move(render_tree)
      );
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
    void start(){
      this->render_tree_->prepare_render(false);
    };
  private:
    std::unique_ptr<std::queue<AppEvent>> queue_system_;
    std::unique_ptr<state::State> state_;
    std::unique_ptr<RenderTree> render_tree_{};
    backend::BaseBackend* backend_; 
  };
};















#endif // FLUXPP_UI_HPP
