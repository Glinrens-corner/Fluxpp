#ifndef XCBBACKEND_HPP
#define XCBBACKEND_HPP
#include "backend/base_backend.hpp"


namespace fluxpp{
  namespace backend{

    namespace xcb{
      class DrawColorCommand: public DrawCommandBase{
	
      };
    }
    
    class XCBAsynchronousInterface: public AsynchronousBackendInterfaceBase {
      std::unique_ptr<DrawCommandBase> get_draw_color_command();
    };
    
    class XCBBackend: public BaseBackend{
    public:
      static inline XCBBackend create(){return XCBBackend{}; };
      std::unique_ptr<AsynchronousBackendInterfaceBase> get_asynchronous_interface(); 
    };
  }
};



#endif //XCBBACKEND_HPP
