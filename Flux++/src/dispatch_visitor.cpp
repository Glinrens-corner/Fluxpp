#include "dispatch_visitor.hpp"
//#include "widget.hpp"

namespace fluxpp{
  namespace visitors {
    void DispatchVisitor::visit(widgets::screen::ScreenBase& screen ){
      screen.accept(*this);
    };
    void DispatchVisitor::visit(widgets::application::ApplicationBase& application ){
      application.accept(*this);
    };
    void DispatchVisitor::visit(widgets::window::WindowBase& window ){
      window.accept(*this);
    };
    void DispatchVisitor::visit(widgets::BaseWidget& widget ){
      widget.accept(*this);
    };

  }// visitors
}// fluxpp
