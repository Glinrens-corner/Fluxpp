#ifndef FLUXPP_WIDGET_EVALUATOR_HPP
#define FLUXPP_WIDGET_EVALUATOR_HPP
#include "event_system.hpp"
#include "bootstrap_ui.hpp"
#include "abstract_backend.hpp"
#include <string>
#include <range/v3/all.hpp>

namespace fluxpp{
  namespace widget_evaluator{
    class WidgetTree;
    

    /*
     *  start() sets the dispatcher to the event system.
     *          all later activations come from that dispatcher
     *  
     */
    class WidgetEvaluator{
    private:
      using event_system_t = fluxpp::event_system::EventSystem;
      using bootstrap_ui_t = fluxpp::bootstrap_ui::BootstrapUi;
      using backend_t = fluxpp::backend::AbstractBackend;
      using string_mapper_t = fluxpp::event_system::LocalStringMapper;
      using path_segment_t = fluxpp::event_system::PathSegment;
      using path_t = fluxpp::event_system::Path;
      using abstract_event_t = fluxpp::event_system::AbstractEvent;
    private:
      using abstract_event_type = abstract_event_t;
      static constexpr  char* self_portal_name_ = "widgets";
    private:
      void handle_events(
          ranges::any_view<
          abstract_event_t*,
          ranges::category::forward>& events );
      void dispatch_event(abstract_event_t*);
    public:
      
      void start();

    private:
      WidgetTree* widget_tree_;
      string_mapper_t * string_mapper_;
      event_system_t * event_system_;
      bootstrap_ui_t * bootstrap_ui_;
      backend_t * backend_;
    };
  }// 
}// fluxpp





#endif //FLUXPP_WIDGET_EVALUATOR_HPP
