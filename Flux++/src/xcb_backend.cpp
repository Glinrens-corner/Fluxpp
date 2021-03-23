#include "backend/xcb_backend.hpp"
#include "backend/xcb_renderer.hpp"
#include "render_tree.hpp"
#include <map>
#include <iostream>

namespace fluxpp{
  namespace backend{
    class XCBBackendImpl{
    public:
      void lock(){};
      
      
      void set_render_tree(RenderTree* render_tree){this->render_tree_ = render_tree;};
      void handle_events(){ this->renderer_.handle_events(this->render_tree_);};
      void unlock(){};

      
    private:
      void insert_commands(
	  std::map<uuid_t,std::unique_ptr<DrawCommandBase>>& command_map ,
	  std::vector<std::unique_ptr<DrawCommandBase>> vec){
	using namespace xcb;
	for (auto & command: vec){
	  std::unique_ptr<DrawCommandBase> tmp{};
	  std::swap(tmp,command );
	  
	  switch(tmp->command_type()){
	  case CommandType::root_node:
	    {
	      auto derived_ptr = dynamic_cast<RootNodeCommand*>( tmp.get());
	      if(!derived_ptr ){
		throw std::runtime_error( "incorrectly labeled node");
	      } else {
		this->root_uuid = derived_ptr->own_uuid_;
		command_map.insert({derived_ptr->own_uuid_ ,std::move(tmp)} );

	      };
	    }
	    break;
	  case CommandType::window_node:
	    {
	      auto derived_ptr = dynamic_cast<WindowNodeCommand*>( tmp.get());
	      if(!derived_ptr ){
		throw std::runtime_error( "incorrectly labeled node");
	      } else {
		command_map.insert({derived_ptr->own_uuid_ ,std::move(tmp)} );
	      };
	    }
	    break;
	  case CommandType::node:
	    {
	      auto derived_ptr = dynamic_cast<NodeCommand*>( tmp.get());
	      if(!derived_ptr ){
		throw std::runtime_error( "incorrectly labeled node");
	      } else {
		command_map.insert({derived_ptr->own_uuid_ ,std::move(tmp)} );
	      };
	    }
	    break;
	  case CommandType::draw_color:
	    {
	      auto derived_ptr = dynamic_cast<DrawColorCommand*>( tmp.get());
	      if(!derived_ptr ){
		throw std::runtime_error( "incorrectly labeled node");
	      } else {
		command_map.insert({derived_ptr->own_uuid_ ,std::move(tmp)} );
	      };
	    }
	    break;

	  case CommandType::draw_text:
	    {
	      auto derived_ptr = dynamic_cast<DrawTextCommand*>( tmp.get());
	      if(!derived_ptr ){
		throw std::runtime_error( "incorrectly labeled node");
	      } else {
		command_map.insert({derived_ptr->own_uuid_ ,std::move(tmp)} );
	      };
	    }
	    break;
	  default:
	    throw std::runtime_error( "unknown node");
	    break;
	  };

	}
	
      };
      
    private:
      RenderTree* render_tree_=nullptr;
      xcb::XCBRenderer renderer_{};
      uuid_t root_uuid;
      std::map<uuid_t,std::unique_ptr<DrawCommandBase>> draw_commands_{};
    };

    XCBSynchronousInterface::XCBSynchronousInterface(XCBBackendImpl * impl):impl(impl){
      this->impl->lock();
    };

    
    XCBSynchronousInterface::~XCBSynchronousInterface(){
      if(this->impl){
	this->impl->unlock();
      };
    };
    void XCBBackend::set_render_tree(RenderTree* tree){
      this->impl->set_render_tree(tree);
    }
    
    std::unique_ptr<DrawCommandBase> XCBAsynchronousInterface
    ::get_draw_color_command(
	uuid_t parent_uuid,
	uuid_t uuid,
	widgets::builtin::Color color
    ){
      return std::unique_ptr<DrawCommandBase>(
	  new xcb::DrawColorCommand{parent_uuid, uuid, color } ); 
    };
    std::unique_ptr<DrawCommandBase> XCBAsynchronousInterface
    ::get_draw_text_command(
	uuid_t parent_uuid,
	uuid_t uuid){
      return std::unique_ptr<DrawCommandBase>(
	  new xcb::DrawTextCommand(
	    parent_uuid,
	    uuid,
	    "test"
	  ) ); 
    };
    
    std::unique_ptr<DrawCommandBase> XCBAsynchronousInterface
    ::get_root_node_command(
	uuid_t node_uuid,
	std::vector<uuid_t> children){
      return std::unique_ptr<DrawCommandBase>(new xcb::RootNodeCommand{node_uuid, std::move(children)} ); 
    };

    std::unique_ptr<DrawCommandBase> XCBAsynchronousInterface
    ::get_window_node_command(
	uuid_t parent_uuid,
	uuid_t uuid,
	std::vector<uuid_t> children){
      return std::unique_ptr<DrawCommandBase>(new xcb::WindowNodeCommand{parent_uuid, uuid, std::move(children)} ); 
    };
    std::unique_ptr<DrawCommandBase> XCBAsynchronousInterface
    ::get_node_command(
	uuid_t parent_uuid,
	uuid_t uuid,
	std::vector<uuid_t> children){
      return std::unique_ptr<DrawCommandBase>(new xcb::NodeCommand{parent_uuid, uuid, std::move(children)} ); 
    };

    void XCBBackend::handle_events(){this->impl->handle_events();};
    XCBBackend XCBBackend::create( ){
      return XCBBackend{ new XCBBackendImpl{} };

    };
    XCBBackend::~XCBBackend(){
      delete this->impl;
    };
    std::unique_ptr<AsynchronousBackendInterfaceBase> XCBBackend::get_asynchronous_interface(){
      return std::unique_ptr<AsynchronousBackendInterfaceBase>(new XCBAsynchronousInterface{});
    };

    std::unique_ptr<SynchronousBackendInterfaceBase> XCBBackend::get_synchronous_interface(){
      return std::unique_ptr<SynchronousBackendInterfaceBase>(new XCBSynchronousInterface{this->impl});
    };
  }// backend
}// fluxpp

