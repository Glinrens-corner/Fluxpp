#ifndef FLUXPP_ABSTRACT_BACKEND_HPP
#define FLUXPP_ABSTRACT_BACKEND_HPP
#include <memory>
#include "util.hpp"
#include "id.hpp"

namespace fluxpp {
  namespace backend {
    struct InstanceIdTag;
    using InstanceId = fluxpp::id::Id<InstanceIdTag>;
    class AbstractDrawCommand{
      
    };
    
    class AbstractDrawCommandFactory {
    public:
      constexpr static fluxpp::util::InterfaceType interface_type = fluxpp::util::InterfaceType::Committing;
      virtual std::unique_ptr<AbstractDrawCommand> get_draw_color_command(
          InstanceId id,
          InstanceId parent_id,
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
