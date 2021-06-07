#ifndef FLUXPP_ABSTRACT_BACKEND_HPP
#define FLUXPP_ABSTRACT_BACKEND_HPP
#include <memory>
#include <map>
#include <optional>
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

    using Position2D =  fluxpp::util::Position2D;

    struct RenderHints  {
    };
    
    using tree_position_t = TreePosition;

    // The Contract of Abstract DrawCommand:
    //   derived types of AbstractDrawCommand are created and allocated by the backend
    //   they have to be transferred back to the backend via dispatch commands
    //   to ensure that the widget tree does not depend on the allocator they are passed as nacked pointers.
    //   But the widget Tree MUST return them to the backend.
    class AbstractDrawCommand{
    public:
      virtual ~AbstractDrawCommand()=0;
    };

    inline AbstractDrawCommand::~AbstractDrawCommand(){};
    
    class AbstractInTreeDrawCommand: public AbstractDrawCommand{
    };

    class Abstract2DDrawCommand: public AbstractInTreeDrawCommand{
    };


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
      
      // Command to draw a possibly infinite color rectangle 
      virtual pair<Abstract2DDrawCommand*, fluxpp::util::Extend2D  >
      get_draw_color_command(
          NodeId id,
          TreePosition tree_position,
          Position2D position,
          RenderHints render_hints,
          fluxpp::util::Color color,
          fluxpp::util::Extend2D size  )=0;

      // get a SetRootCommand...
      // the command to set a node as root for this backend 
      virtual pair<AbstractDrawCommand*, fluxpp::util::None > get_set_root_command(
          NodeId id,
          RenderHints render_hints
      ) =0;

      // Command to add a Window
      virtual pair<Abstract2DDrawCommand*, WindowSettings> get_add_window_command(
          NodeId id,
          TreePosition tree_hint,
          Position2D position_hint,
          RenderHints render_hints,
          fluxpp::util::Extend2D size
      ) =0;
      
      // Command to change the position of a node in the render tree.
      virtual pair<AbstractInTreeDrawCommand*, fluxpp::util::None >
      get_move_in_tree_command(
          NodeId id,
          TreePosition tree_hint,
          RenderHints render_hints
      )=0;

      // COmmand to create a generic 2D node.
      virtual pair<Abstract2DDrawCommand*, fluxpp::util::None>
      get_create_2D_node_command(
          NodeId id,
          TreePosition tree,
          Position2D position,
          RenderHints render_hints
      )=0;
      

      // get a backend specific command
      virtual pair<AbstractDrawCommand*, void*>
      get_create_any_command(
          NodeId id,
          const std::string& name,
          void * data) = 0;
    };

    class AbstractDrawCommandDispatcher{
    public:
      virtual void dispatch_commands(
          std::unordered_map<NodeId,AbstractDrawCommand*>,
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
