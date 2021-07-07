#include "widget_evaluator.hpp"
#include <range/v3/all.hpp>
#include <algorithm>
#include <list>

namespace fluxpp{
  namespace widget_evaluator{
    
    void WidgetEvaluator::start(){
      this->event_system_->get_portal(
          path_segment_t{ {
              this->
                string_mapper_->
                get_interface()
                .map_string(this->self_portal_name_)
                }
          })
        .set_buffered_dispatcher(
            [](abstract_event_t* event)->bool{
              return static_cast<bool>(dynamic_cast<fluxpp::event_system::RenderRequestEvent*>(event));
            }
            ,
            [this](ranges::any_view<abstract_event_t*, ranges::category::forward>& events)->void{
            this->handle_events(events);
          });;
      
    };

    void WidgetEvaluator::handle_events(ranges::any_view<abstract_event_type*, ranges::category::forward>& events){

      std::list<abstract_event_type*> reverse_events;
      for(abstract_event_type* event: events){
        reverse_events.push_front(event);
      };
      for (abstract_event_type* event: reverse_events){
        this->dispatch_event(event);
      };
    };

    void WidgetEvaluator::dispatch_event( abstract_event_type* event){
      using fluxpp::event_system::Path;
      const Path&  target = event->target();
      if(target.content_.size() ==0 ){
        this->bootstrap_ui_->warning(0, fmt::format("encountered empty_target {}",  *event ));
        return;
      };
      if (target.content_.size() > 1 ){
        this->bootstrap_ui_->warning(0, fmt::format("sorry currently  only direct dispatch to the widget evaluator is supported. {}",  *event ));
        return;
      };

      //      RenderVisitor( this).dispatch(event);
      
      
    };
  }//widget_evaluator

}//fluxpp
