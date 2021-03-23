#ifndef FLUXPP_BASE_BACKEND_FWD_HPP
#define FLUXPP_BASE_BACKEND_FWD_HPP
#include <vector>
#include "uuid.hpp"
#include "color.hpp"

namespace fluxpp{
  namespace backend{
    enum class CommandType{
      draw_color,
      draw_text,
      root_node,
      window_node,
      node
    };
    class DrawCommandBase{
    public:
      DrawCommandBase(CommandType command_type) :command_type_(command_type){};
	// the base class needs a virtual function so dynamic_cast works...
	virtual void foo(){};
      
      CommandType command_type()const {return this->command_type_;}; 
    private:
      CommandType command_type_; 
    };

    class SynchronousBackendInterfaceBase{
    public:
    };
    
    class AsynchronousBackendInterfaceBase{
    public:
      virtual std::unique_ptr<DrawCommandBase> get_draw_color_command(
	  uuid_t parent_uuid,
	  uuid_t uuid,
	  widgets::builtin::Color color
      ) = 0;
      virtual std::unique_ptr<DrawCommandBase> get_draw_text_command(
	  uuid_t parent_uuid,
	  uuid_t uuid) =0;
      virtual std::unique_ptr<DrawCommandBase> get_root_node_command(
	  uuid_t uuid,
	  std::vector<uuid_t> children) = 0;
      virtual std::unique_ptr<DrawCommandBase> get_window_node_command(
	  uuid_t parend_uuid,
	  uuid_t window_uuid,
	  std::vector<uuid_t> children ) = 0;
      virtual std::unique_ptr<DrawCommandBase> get_node_command(
	  uuid_t parent_uuid,
	  uuid_t node_uuid,
	  std::vector<uuid_t> children) = 0;
    };
  }//backend
}//fluxpp


#endif
