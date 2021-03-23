#ifndef FLUXPP_BASE_BACKEND_HPP
#define FLUXPP_BASE_BACKEND_HPP
#include <memory>
#include "backend/base_backend_fwd.hpp"
#include "render_tree_fwd.hpp"
#include <vector>

namespace fluxpp{
  namespace backend{

    class BaseBackend{
    public:
      virtual void set_render_tree(RenderTree * ) =0; 
      virtual void handle_events() =0;
      virtual std::unique_ptr<AsynchronousBackendInterfaceBase> get_asynchronous_interface()=0;
      virtual std::unique_ptr<SynchronousBackendInterfaceBase> get_synchronous_interface()=0;
    };
  }// backend
}//fluxpp



#endif //FLUXPP_BASE_BACKEND_HPP
