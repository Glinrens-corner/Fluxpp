#ifndef FLUXPP_DISPATCH_VISITOR_HPP
#define FLUXPP_DISPATCH_VISITOR_HPP
#include <queue>
#include "app_event.hpp"
#include "widget_fwd.hpp"
#include "gui_event.hpp"

namespace fluxpp{
  namespace visitors {
    class DispatchVisitor{
      
    public:
      template<class event_t>
      DispatchVisitor(event_t* event, std::queue<widgets::AppEvent>*app_queue)
	:type_( event->event_type())
	,event_(static_cast<void*>(event))
	,app_queue_(app_queue){};
    public:
      void visit( widgets::screen::ScreenBase& screen);
      void visit( widgets::application::ApplicationBase& application);
      void visit( widgets::window::WindowBase& screen);
      void visit( widgets::BaseWidget& screen);

      template<class event_t>
      bool handle_handler(widgets::EventHandler<widgets::AppEvent, event_t>& event_handler){
	if( event_t::event_type() == this->type_){
	  this->app_queue_->push(std::move(event_handler(*reinterpret_cast<event_t*>( this->event_)).event));
	};
	return true;
      };
      
    private:
      events::EventType type_;
      void * event_;
      std::queue<widgets::AppEvent>* app_queue_;
    };
  }// visitors
}// fluxpp
#endif 
