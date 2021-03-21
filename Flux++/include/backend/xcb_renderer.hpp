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
      class XCBRenderer {
      public:
	XCBRenderer();
	bool handle_events();
	
	void render ();
	void update_commands(
	    std::map<uuid_t,std::unique_ptr<DrawCommandBase>>* commands ,
	    uuid_t  root);
	~XCBRenderer();
      private:
	uuid_t root_{};
	std::map<uuid_t,std::unique_ptr<DrawCommandBase>>* commands_;	
	xcb_connection_t *connection_;
	std::unique_ptr<XCBWindowRenderer> window_renderer_ {};
      };
      
    } // xcb
  }//backend
}//fluxpp



#endif //FLUXPP_XCB_RENDERER_HPP
