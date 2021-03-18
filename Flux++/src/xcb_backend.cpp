#include "backend/xcb_backend.hpp"
#include <map>
#include <iostream>

namespace fluxpp{
  namespace backend{
    class XCBBackendImpl{
    public:
      void lock(){};
      void update_commands(std::vector<std::unique_ptr<DrawCommandBase>> vec){
	std::cout<< "updating draw_commands"<< std::endl;
	this->draw_commands_.clear();
	this->insert_commands(this->draw_commands_, std::move(vec));
      };
      void unlock(){};
    private:
      void insert_commands(std::map<uuid_t,std::unique_ptr<DrawCommandBase>>& command_map , std::vector<std::unique_ptr<DrawCommandBase>> vec){
      };
    private:
      std::map<uuid_t,std::unique_ptr<DrawCommandBase>> draw_commands_{};
    };

    XCBSynchronousInterface::XCBSynchronousInterface(XCBBackendImpl * impl):impl(impl){
      this->impl->lock();
    };

    void XCBSynchronousInterface::update_commands(std::vector<std::unique_ptr<DrawCommandBase> > vec ){
      this->impl->update_commands(std::move(vec));
    };
    
    XCBSynchronousInterface::~XCBSynchronousInterface(){
      if(this->impl){
	this->impl->unlock();
      };
    };

    
    std::unique_ptr<DrawCommandBase> XCBAsynchronousInterface
    ::get_draw_color_command(
	uuid_t parent_uuid,
	uuid_t uuid){
      return std::unique_ptr<DrawCommandBase>(new xcb::DrawColorCommand{} ); 
    };
    std::unique_ptr<DrawCommandBase> XCBAsynchronousInterface
    ::get_draw_text_command(
	uuid_t parent_uuid,
	uuid_t uuid){
      return std::unique_ptr<DrawCommandBase>(new xcb::DrawColorCommand{} ); 
    };
    
    std::unique_ptr<DrawCommandBase> XCBAsynchronousInterface
    ::get_root_node_command(
	uuid_t node_uuid,
	std::vector<uuid_t> children){
      return std::unique_ptr<DrawCommandBase>(new xcb::DrawColorCommand{} ); 
    };

    std::unique_ptr<DrawCommandBase> XCBAsynchronousInterface
    ::get_window_node_command(
	uuid_t parent_uuid,
	uuid_t uuid,
	std::vector<uuid_t> children){
      return std::unique_ptr<DrawCommandBase>(new xcb::DrawColorCommand{} ); 
    };
    std::unique_ptr<DrawCommandBase> XCBAsynchronousInterface
    ::get_node_command(
	uuid_t parent_uuid,
	uuid_t uuid,
	std::vector<uuid_t> children){
      return std::unique_ptr<DrawCommandBase>(new xcb::DrawColorCommand{} ); 
    };

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

