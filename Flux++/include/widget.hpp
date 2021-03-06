#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <mem_comparable_closure.hpp>
#include <string>
#include <tuple>
#include <array>

namespace fluxpp{ 
  /* 
 namespace widget{
    using std::tuple;
    using std::array;
    using transparent_closure::Fun;

    struct BaseAppEvent{
      std::string target;  
    };
    
    struct GuiEvent{};
    struct WidgetReturnType{};
    
    template<class return_t>
    struct Filter {
      std::string target;
      return_t (*fn)(void*); 
    };

    template<class app_event_t>
    struct EventHandler {
      Fun<app_event_t,GuiEvent> function;
    };


    
    template<class app_event_t, int napp_handlers, class ... state_ts>
    class Widget{
    private:
      tuple<Filter<state_ts>...> filters ;
      Fun< WidgetReturnType, state_ts...> render;
      array<EventHandler<app_event_t>, napp_handlers> app_handlers;
    };
    // Widget( filters, renderfn, event_handlers);
}// widget
*/
  
} // fluxpp





#endif
