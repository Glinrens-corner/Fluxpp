#ifndef XCBBACKEND_HPP
#define XCBBACKEND_HPP
#include "backend/base_backend.hpp"
#include "backend/xcb_renderer_fwd.hpp"
#include "widget_fwd.hpp"
#include <vector>

namespace fluxpp{
  namespace backend{

    namespace xcb{
      class DrawColorCommand: public DrawCommandBase{
      public:
	DrawColorCommand(
	    uuid_t parent_uuid,
	    uuid_t widget_uuid,
	    widgets::builtin::Color color)
	  :DrawCommandBase(CommandType::draw_color)
	  ,parent_uuid_(parent_uuid)
	  ,own_uuid_(widget_uuid)
	  ,color_(color){ };
      public:
	uuid_t parent_uuid_;
	uuid_t own_uuid_;
	widgets::builtin::Color color_;
      };
      
      class DrawTextCommand: public DrawCommandBase{
      public:
	DrawTextCommand(
	    uuid_t parent_uuid,
	    uuid_t widget_uuid,
	    std::string text)
	  :DrawCommandBase(CommandType::draw_text)
	  ,parent_uuid_(parent_uuid)
	  ,own_uuid_(widget_uuid)
	  ,text_(std::move(text)){ };
      public:
	uuid_t parent_uuid_;
	uuid_t own_uuid_;
	std::string text_;
      };
      class NodeCommand: public DrawCommandBase{
      public:
	NodeCommand(
	    uuid_t parent_uuid,
	    uuid_t widget_uuid,
	    std::vector<uuid_t> children)
	  :DrawCommandBase(CommandType::node)
	  ,parent_uuid_(parent_uuid)
	  ,own_uuid_(widget_uuid)
	  ,children_(std::move(children)){ };
      public:
	uuid_t parent_uuid_;
	uuid_t own_uuid_;
	std::vector<uuid_t> children_;
      };
      
      class WindowNodeCommand: public DrawCommandBase{
      public:
	WindowNodeCommand(
	    uuid_t parent_uuid,
	    uuid_t widget_uuid,
	    std::vector<uuid_t> children)
	  :DrawCommandBase(CommandType::window_node)
	  ,parent_uuid_(parent_uuid)
	  ,own_uuid_(widget_uuid)
	  ,children_(std::move(children)){ };
      public:
	uuid_t parent_uuid_;
	uuid_t own_uuid_;
	std::vector<uuid_t> children_;
      };
      class RootNodeCommand: public DrawCommandBase{
      public:
        RootNodeCommand(
	    uuid_t widget_uuid,
	    std::vector<uuid_t> children)
	  :DrawCommandBase(CommandType::root_node)
	  ,own_uuid_(widget_uuid)
	  ,children_(std::move(children)){ };
      public:
	uuid_t own_uuid_;
	std::vector<uuid_t> children_;
      };

    }
    
    class XCBAsynchronousInterface: public AsynchronousBackendInterfaceBase {
      std::unique_ptr<DrawCommandBase> get_draw_color_command(
	  uuid_t parent_uuid,
	  uuid_t node_uuid,
	  widgets::builtin::Color color );
      std::unique_ptr<DrawCommandBase> get_draw_text_command(
	  uuid_t parent_uuid,
	  uuid_t node_uuid);
      std::unique_ptr<DrawCommandBase> get_root_node_command(
	  uuid_t uuid,
	  std::vector<uuid_t> children);
      std::unique_ptr<DrawCommandBase> get_window_node_command(
	  uuid_t parent_uuid,
	  uuid_t node_uuid,
	  std::vector<uuid_t> children);
      std::unique_ptr<DrawCommandBase> get_node_command(
	  uuid_t parent_uuid,
	  uuid_t node_uuid,
	  std::vector<uuid_t> children);
    };

    class XCBBackendImpl;
    
    class XCBSynchronousInterface: public SynchronousBackendInterfaceBase {
    public:
      XCBSynchronousInterface(XCBBackendImpl* impl);
      ~XCBSynchronousInterface();
      
      XCBSynchronousInterface(const XCBSynchronousInterface& ) = delete;
      XCBSynchronousInterface(XCBSynchronousInterface&& other):impl(other.impl){other.impl=nullptr;};
      XCBSynchronousInterface& operator= (const XCBSynchronousInterface& ) = delete;
      XCBSynchronousInterface& operator= ( XCBSynchronousInterface&& other){
	auto tmp = other.impl;
	other.impl = this->impl;
	this->impl = tmp;
      };
      void update_commands(std::vector<std::unique_ptr<DrawCommandBase>> );
      
    private:
      XCBBackendImpl* impl;
    };

    
    class XCBBackend: public BaseBackend{
    public:
      static XCBBackend create();
      XCBBackend(XCBBackendImpl * impl ): impl(impl) {};

      XCBBackend(const XCBBackend& ) =delete;
      XCBBackend(XCBBackend&& other):impl(other.impl){
	other.impl = nullptr;
      };
      XCBBackend& operator=(const XCBBackend& ) = delete;
      void handle_events();
      XCBBackend& operator=(XCBBackend&& other){
	auto tmp = other.impl;
	other.impl = this->impl;
	this->impl = tmp;
	return *this;
      };

      ~XCBBackend();
      std::unique_ptr<AsynchronousBackendInterfaceBase> get_asynchronous_interface(); 
      std::unique_ptr<SynchronousBackendInterfaceBase> get_synchronous_interface();
    private:
      XCBBackendImpl * impl;
    };
  }
};



#endif //XCBBACKEND_HPP
