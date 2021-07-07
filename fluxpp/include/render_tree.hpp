#ifndef FLUXPP_WIDGET_EVALUATOR_HPP
#define FLUXPP_WIDGET_EVALUATOR_HPP
#include "event_system.hpp"
#include "bootstrap_ui.hpp"
#include "abstract_backend.hpp"

namespace fluxpp{
  namespace widget_evaluator{
    class WidgetEvaluator{
    private:
      using event_system_t = fluxpp::event_system::EventSystem;
      using bootstrap_ui_t = fluxpp::bootstrap_ui::BootstrapUi;
      using backend_t = fluxpp::backend::AbstractBackend;
      using string_mapper_t = fluxpp::event_system::LocalStringMapper;
    public:

      


    private:
      string_mapper_t * string_mapper_;
      event_system_t * event_system_;
      bootstrap_ui_t * boostrap_ui_;
      backend_t * backend_;
    };
  }// 
}// fluxpp





#endif //FLUXPP_RENDER_TREE_HPP
