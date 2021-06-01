#ifndef FLUXPP_ABSTRACT_BACKEND_HPP
#define FLUXPP_ABSTRACT_BACKEND_HPP
#include <memory>
#include <vector>
#include "util.hpp"
#include "id.hpp"

namespace fluxpp {
  namespace backend {
    struct InstanceIdTag;
    using InstanceId = fluxpp::id::Id<InstanceIdTag>;

    class AbstractDrawCommand{
    public:
      virtual ~AbstractDrawCommand()=0;
    };
    
    inline AbstractDrawCommand::~AbstractDrawCommand(){};
    
    class AbstractDrawCommandFactory {
    public:
      constexpr static fluxpp::util::InterfaceType interface_type = fluxpp::util::InterfaceType::Committing;
      virtual AbstractDrawCommand* get_draw_color_command(
          InstanceId id,
          InstanceId parent_id,
          fluxpp::util::Color color,
          fluxpp::util::Extend2D size  )=0;
    };

    class AbstractDrawCommandDispatcher{
    public:
      virtual void dispatch_commands(
          std::vector<AbstractDrawCommand*>,
          std::unique_ptr<AbstractDrawCommandFactory>) =0; 
    };

    class AbstractBackend {
    public:
      virtual std::unique_ptr<AbstractDrawCommandFactory> get_draw_command_factory() = 0;

      virtual std::unique_ptr<AbstractDrawCommandDispatcher> get_draw_command_dispatcher() = 0;
    };


  } // fluxpp
} //fluxpp







#endif FLUXPP_ABSTRACT_BACKEND_HPP
