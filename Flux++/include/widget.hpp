#ifndef WIDGET_HPP
#define WIDGET_HPP
#include <string>
#include <tuple>
#include <array>
#include <mem_comparable_closure.hpp>
#include "gui_event.hpp"

namespace fluxpp{ 
  
 namespace widgets{
   using mem_comparable_closure::Function;
   using std::tuple;
   using std::array;
   
   struct AppEvent{
     std::string target;  
   };

   template<class return_t>
   struct Filter {
     Filter( std::string target_):target(std::move(target_)){ };
     std::string target;
     //     return_t (*fn)(void*); 
   };
   
   template<class app_event_t, class gui_event_t>
   struct EventHandler {
     Function<app_event_t,gui_event_t> function;
   };
   
   namespace screen {
     
   }
   
   namespace window {

   }
   
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


   class ApplicationReturnContainerBase{

   };

   template<class ...arg_ts>
   class ApplicationReturnContainer : public ApplicationReturnContainerBase{
   public:
     ApplicationReturnContainer(arg_ts ... args ): widgets_(std::make_tuple(std::move(args)... )) {};
   private:
     std::tuple<arg_ts...> widgets_;
   };
   
   template < class ...arg_ts  >
   ApplicationReturnContainerBase* make_application_return_container(   arg_ts... args ) {
     return new ApplicationReturnContainer<arg_ts...>{std::move(args)... };
   };

   

   template <WidgetClass widget_class>
   struct get_render_return_type;

   
   template<>
   struct get_render_return_type<WidgetClass::application>{
     using type = int;
     
   };
   
   template<WidgetClass widget_class_, class ... Ts>
   class Widget;

   
   template<WidgetClass widget_class_,  class app_event_t, class ... event_ts , class ... state_ts>
   class Widget<widget_class_, tuple<EventHandler<app_event_t, event_ts>...>, state_ts...>{
   public:
     using render_return_type = typename get_render_return_type<widget_class_>::type;
   private:
     tuple<Filter<state_ts>...> filters ;
     Function< render_return_type, state_ts...> render;
     tuple<EventHandler<app_event_t, event_ts>...> event_handlers;
   };

   template<class ... state_ts>
   class Widget<WidgetClass::application,  state_ts...>{
   public:
     using render_return_type = typename get_render_return_type<WidgetClass::application>::type;
     using filter_tuple_type = tuple<Filter<state_ts>...>;
     using render_function_type = Function<render_return_type, state_ts...>;
   private:
     tuple<Filter<state_ts>...> filters ;
     Function<render_return_type, state_ts...> render_function;
   };
   
   namespace app{
     template<class ...state_ts>
     using App = Widget< WidgetClass::application, state_ts...>;

   } //app
 }// widget
  
} // fluxpp





#endif
