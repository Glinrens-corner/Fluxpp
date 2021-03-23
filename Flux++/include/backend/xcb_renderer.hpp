#ifndef FLUXPP_XCB_RENDERER_HPP
#define FLUXPP_XCB_RENDERER_HPP
#include "backend/xcb_backend.hpp" 
#include <xcb/xcb.h>
#include <map>
#include "uuid.hpp"
#include "widget.hpp"

namespace fluxpp {
  namespace backend{
    namespace xcb {
      class XCBWindowRenderer{
      public:
	
	XCBWindowRenderer(
	    xcb_window_t window,
	    xcb_screen_t * screen,
	    xcb_connection_t* connection,
	    widgets::Size size)
	  :window_(window),
	   screen_(screen),
	   connection_(connection),
	   size_(size){};
	~XCBWindowRenderer();
	static XCBWindowRenderer  create(xcb_connection_t * connection, widgets::Size);
	void render(
	    std::map<uuid_t,std::unique_ptr<DrawCommandBase>>& commands ,
	    uuid_t  root);
	void clear(uint8_t* , widgets::Size);
	void render_command(DrawColorCommand* ,uint8_t*, widgets::Size);
	void render_command(DrawTextCommand* ,uint8_t*, widgets::Size);
      private:
	widgets::Size size_;
	xcb_window_t window_;
	xcb_screen_t * screen_; 
	xcb_connection_t * connection_;
      };

      class CommandStorage{
      public:
	void update_commands(std::vector<std::unique_ptr<DrawCommandBase>> vec){
	  this->commands_.clear();
	  this->insert_commands(this->commands_, std::move(vec));
	  
	};
	uuid_t root_uuid(){return this->root_;};
	std::map<uuid_t,std::unique_ptr<DrawCommandBase>>& commands(){
	  return this->commands_;
	}
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
		this->root_ = derived_ptr->own_uuid_;
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
	uuid_t root_{};
	std::map<uuid_t,std::unique_ptr<DrawCommandBase>> commands_{};
	
      };

      
      class XCBRenderer {
      public:
	XCBRenderer();
	bool handle_events(RenderTree *);
	
	void render ();
	void update_commands(
	    std::map<uuid_t,std::unique_ptr<DrawCommandBase>>* commands ,
	    uuid_t  root);
	~XCBRenderer();
      private:
        CommandStorage commands_;	
	xcb_connection_t *connection_;
	std::unique_ptr<XCBWindowRenderer> window_renderer_ {};
      };
      
    } // xcb
  }//backend
}//fluxpp



#endif //FLUXPP_XCB_RENDERER_HPP
