#ifndef FLUXPP_ABSTRACT_BACKEND_HPP
#define FLUXPP_ABSTRACT_BACKEND_HPP
#include <memory>
#include "util.hpp"
#include "id.hpp"

namespace fluxpp {
  namespace backend {
    using ::fluxpp::id::id_t;
    class AbstractDrawCommand{
      

    };
    
    class AbstractDrawCommandFactory {
    public:
      constexpr static fluxpp::util::InterfaceType interface_type = fluxpp::util::InterfaceType::Committing;
      virtual std::unique_ptr<AbstractDrawCommand> get_draw_color_command(
          id_t id,
          id_t parent_id,
          fluxpp::util::Color color,
          fluxpp::util::Extend2D size  )=0;
      
    };
    
    class AbstractBackend {
    public:
     virtual std::unique_ptr<AbstractDrawCommandFactory> get_draw_command_factory() = 0;
    };

  } // fluxpp
} //fluxpp







#endif FLUXPP_ABSTRACT_BACKEND_HPP
