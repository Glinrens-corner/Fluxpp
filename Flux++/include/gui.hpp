#ifndef GUI_HPP
#define GUI_HPP
#include <memory>
#include "state.hpp"
#include "backend/base_backend.hpp"
#include "widget.hpp"

namespace fluxpp{
  class Ui{
  public:
    Ui(backend::BaseBackend* backend):backend_(backend), state_(std::make_unique<state::State>()) {};
    template<class T>
    void add_state_slice(const std::string& position, state::StateSlice<T> slice){
      this->state_->add_slice(position, std::unique_ptr<state::BaseStateSlice>(new state::StateSlice<T>(slice) )  );
    };
    template<class ...Ts>
    void set_application_widget(widgets::application::Application<Ts...> application){
      if( this->application_){
	throw std::exception();
      };
      this->application_ = std::make_unique( application );
    };
  private:
    std::unique_ptr<state::State> state_;
    std::unique_ptr<widgets::BaseWidget> application_{};
    backend::BaseBackend* backend_; 
  };
};















#endif // GUI_HPP
