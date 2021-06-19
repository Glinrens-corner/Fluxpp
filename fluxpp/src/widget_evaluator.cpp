#include "widget_evaluator.hpp"
#include <range/v3/all.hpp>

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
              if (dynamic_cast<fluxpp::event_system::RenderRequestEvent*>(event)){
                return true;
              } else {
                return false;
              };
            }
            ,
            [this](ranges::any_view<abstract_event_t*, ranges::category::forward>& events)->void{
            this->handle_events(events);
          });;
      
    };

    void WidgetEvaluator::handle_events(ranges::any_view<abstract_event_type*, ranges::category::forward>& events){


    };


  }//widget_evaluator

}//fluxpp
