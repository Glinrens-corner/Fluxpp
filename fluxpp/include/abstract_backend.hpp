#ifndef FLUXPP_ABSTRACT_BACKEND_HPP
#define FLUXPP_ABSTRACT_BACKEND_HPP
#include <memory>
#include <vector>
#include "util.hpp"
#include "id.hpp"

namespace fluxpp {
  namespace backend {
    struct NodeIdTag;
    using NodeId = fluxpp::id::Id<NodeIdTag>;
    struct TreePosition {
      NodeId parent;
      std::size_t position;
    };
    using tree_position_t = TreePosition;

    class AbstractDrawCommand{
    public:
      virtual ~AbstractDrawCommand()=0;
    };
    
    inline AbstractDrawCommand::~AbstractDrawCommand(){};

    struct WindowSettings{
      fluxpp::util::Position2D position;
      fluxpp::util::Extend2D size;
    };
    
    class AbstractDrawCommandFactory {
    private:
      // just to avoid to always type std 
      template<class T1, class T2>
      using pair = std::pair<T1,T2>;
    public:
      constexpr static fluxpp::util::InterfaceType interface_type = fluxpp::util::InterfaceType::Committing;
      
      virtual pair<AbstractDrawCommand*, fluxpp::util::Extend2D  >
      get_draw_color_command(
          NodeId id,
          tree_position_t tree_position,
          fluxpp::util::Color color,
          fluxpp::util::Position2D position,
          fluxpp::util::Extend2D size  )=0;

      virtual pair<AbstractDrawCommand*, fluxpp::util::None > get_set_root_command(
          NodeId id
      ) =0;
      
      virtual pair<AbstractDrawCommand*, WindowSettings> get_add_window_command(
          NodeId id,
          tree_position_t tree_position,
          fluxpp::util::Position2D position,
          fluxpp::util::Extend2D size
      ) =0;
      
      virtual pair<AbstractDrawCommand*, fluxpp::util::None >
      get_move_in_tree_command(
          NodeId id,
          tree_position_t tree_position
          )=0;

      virtual pair<AbstractDrawCommand*, fluxpp::util::None>
      get_create_node_command(
          NodeId id,
          tree_position_t tree_position)=0;

      virtual pair<AbstractDrawCommand*, void*>
      get_create_any_command(
          NodeId id,
          tree_position_t tree_position,
          const std::string& name,
          void * data) = 0;    
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
