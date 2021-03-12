#ifndef FLUXPP_RENDER_VISITOR_HPP
#define FLUXPP_RENDER_VISITOR_HPP
#include "backend/base_backend.hpp"
#include "state.hpp"


namespace fluxpp{
  namepsace visitors{
    using uuid_t = boost::uuids::uuid;
    class RenderVisitor{
     
    public:
      visit_base(uuid_t)
    private:
      backend::AsynchronousBackendInterfaceBase * backend_aifc;
      state::SynchronousStateInterface* state_ifc;
      std::set<uuid_t,widgets> ;

      
    };
    

  }

}
#endif //FLUXPP_RENDER_VISITOR_HPP
