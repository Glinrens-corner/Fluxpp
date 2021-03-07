#ifndef WIDGET_HPP
#define WIDGET_HPP
#include <string>
#include <tuple>
#include <array>
#include <mem_comparable_closure.hpp>
#include "gui_event.hpp"

namespace fluxpp{ 
  
 namespace widget{
   using mem_comparable_closure::Fun;
   using std::tuple;
   using std::array;
   
   struct AppEvent{
     std::string target;  
   };
   

   
   struct WidgetReturnType{};
   
   template<class return_t>
   struct Filter {
     std::string target;
     return_t (*fn)(void*); 
   };
   
   template<class app_event_t, class gui_event_t>
   struct EventHandler {
     Fun<app_event_t,gui_event_t> function;
   };
   
   enum class WidgetClass{
     application,
     display,
     window_visual_2d,
     widget_visual_2d,
     widget_leaf_visual_2d,
     none
   };

   template<WidgetClass widget_class>
   struct get_render_return_type;

   // //   template<>
   // struct get_render_return_type
   
   template<WidgetClass widget_class_, class ... Ts>
   class Widget;

   
   template<WidgetClass widget_class_,  class app_event_t, class ... event_ts , class ... state_ts>
   class Widget<widget_class_, tuple<EventHandler<app_event_t, event_ts>...>, state_ts...>{
   public:
     using render_return_type = typename get_render_return_type<widget_class_>::type;
   private:
     tuple<Filter<state_ts>...> filters ;
     Fun< render_return_type, state_ts...> render;
     tuple<EventHandler<app_event_t, event_ts>...> event_handlers;
   };

   
   // Widget( filters, renderfn, event_handlers);
 }// widget
  
} // fluxpp





#endif
