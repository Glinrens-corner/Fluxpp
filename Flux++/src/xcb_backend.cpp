#include "backend/xcb_backend.hpp"

namespace fluxpp{
  namespace backend{
    std::unique_ptr<DrawCommandBase> XCBAsynchronousInterface::get_draw_color_command(){
      return std::unique_ptr<DrawCommandBase>(new xcb::DrawColorCommand{} ); 
    };

    std::unique_ptr<AsynchronousBackendInterfaceBase> XCBBackend::get_asynchronous_interface(){
      return std::unique_ptr<AsynchronousBackendInterfaceBase>(new XCBAsynchronousInterface{});
    };
  }// backend
}// fluxpp

