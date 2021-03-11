#ifndef BASEBACKEND_HPP
#define BASEBACKEND_HPP
#include <memory>
namespace fluxpp{
  namespace backend{
    class DrawCommandBase{};

    class AsynchronousBackendBaseImpl{};
    class SynchronousBackendInterfaceBase{

    };
    
    class AsynchronousBackendInterfaceBase{
    public:
      virtual std::unique_ptr<DrawCommandBase> get_draw_color_command( ) = 0;
    };
    
    class BaseBackend{
    public:
      virtual std::unique_ptr<AsynchronousBackendInterfaceBase> get_asynchronous_interface()=0;

    };
  }
};



#endif //BASEBACKEND_HPP
