#ifndef BASEBACKEND_HPP
#define BASEBACKEND_HPP
#include <memory>
#include "uuid.hpp"
#include <vector>
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
      CommandType command_type()const {return this->command_type_;}; 
    private:
      CommandType command_type_; 
    };

    class SynchronousBackendInterfaceBase{
    public:
      virtual void update_commands(std::vector<std::unique_ptr<DrawCommandBase> > vec ) =0;

    };
    
    class AsynchronousBackendInterfaceBase{
    public:
      virtual std::unique_ptr<DrawCommandBase> get_draw_color_command(
	  uuid_t parent_uuid,
	  uuid_t uuid) = 0;
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
    
    class BaseBackend{
    public:
      virtual std::unique_ptr<AsynchronousBackendInterfaceBase> get_asynchronous_interface()=0;
      virtual std::unique_ptr<SynchronousBackendInterfaceBase> get_synchronous_interface()=0;
    };
  }// backend
}//fluxpp



#endif //BASEBACKEND_HPP
