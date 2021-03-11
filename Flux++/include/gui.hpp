#ifndef GUI_HPP
#define GUI_HPP
#include <memory>
#include <queue>
#include "state.hpp"
#include "backend/base_backend.hpp"
#include "widget.hpp"

namespace fluxpp{
  class WidgetNode;
  
  class RenderTree{
  public:
    RenderTree(std::unique_ptr<widgets::BaseWidget>&&,
	       std::queue<AppEvent>*,
	       backend::BaseBackend*,
	       state::State*);
    RenderTree(const RenderTree &) =delete;
    void prepare_render();
    ~RenderTree();
  private:
    class RenderTreeImpl;
    RenderTreeImpl* impl;
  };
    
  class Ui{
  public:
    Ui(backend::BaseBackend* backend):backend_(backend), state_(std::make_unique<state::State>()) {};
    template<class T>
    void add_state_slice(const std::string& position, state::StateSlice<T> slice){
      this->state_->add_slice(position, std::unique_ptr<state::BaseStateSlice>(new state::StateSlice<T>(slice) )  );
    };
    template<class ...Ts>
    void set_application_widget(widgets::application::Application<Ts...> application){
      if( this->render_tree_){
	// there was already an application set.
	throw std::exception();
      };
      this->render_tree_ = std::unique_ptr<RenderTree>( new RenderTree(std::unique_ptr<widgets::BaseWidget>(new widgets::application::Application{ application} ), nullptr, nullptr, nullptr));
    };
    
  private:
    std::unique_ptr<state::State> state_;
    std::unique_ptr<RenderTree> render_tree_{};
    backend::BaseBackend* backend_; 
  };
};















#endif // GUI_HPP
