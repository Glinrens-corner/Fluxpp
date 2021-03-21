#include "backend/xcb_renderer.hpp"
#include <xcb/render.h>
#include <xcb/xcb_image.h>
#include <thread>
#include <chrono>
#include <cassert>
#include <iostream>
#include <stack>
#include "widget.hpp"
#include "gui_event.hpp"

namespace fluxpp {
  namespace backend{
    namespace xcb {
      using events::Coordinate;
      using widgets::Size;


      using path_stack_t = std::stack<
	std::pair<std::size_t,uuid_t>,std::vector<std::pair<std::size_t,uuid_t> > > ;
      
      class PositionStack{
	PositionStack(){
	  this->stack_.push(Coordinate{0,0});
	};
	void push(Coordinate coord){ this->stack_.push(this->stack_.top() +coord) ;};
	const Coordinate& top()const{ return this->stack_.top();};
        void pop(){ this->stack_.pop();};
      private:
	  std::stack<Coordinate, std::vector<Coordinate>> stack_{};
      };

      namespace detail {
	bool query_render_extension(xcb_connection_t *connection ){
	  const xcb_query_extension_reply_t * render_extension =
	    xcb_get_extension_data(connection, &xcb_render_id);
	  return static_cast<bool>(render_extension);
	};
      };
      XCBWindowRenderer XCBWindowRenderer::create(xcb_connection_t * connection, Size size) {
        xcb_screen_t*  screen = xcb_setup_roots_iterator(
	    xcb_get_setup(connection)).data;
	if (detail::query_render_extension( connection) ){
	  std::cout << "RenderExtension detected"<< std::endl;
 	}else {
	  throw std::exception();
	};
	xcb_window_t win = xcb_generate_id(connection);
	uint32_t mask=XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	uint32_t values[2];
	values[0] = screen->white_pixel;
	values[1]= XCB_EVENT_MASK_EXPOSURE ;
	xcb_create_window(connection,
			  XCB_COPY_FROM_PARENT,
			  win,
			  screen->root,
			  0, 0,
			  size.width, size.height,
			  10,
			  XCB_WINDOW_CLASS_INPUT_OUTPUT,
			  screen->root_visual,
			  mask, values);
	xcb_map_window(connection, win);
	xcb_flush(connection);
	return {win,screen,connection , size};
      };

      XCBWindowRenderer::~XCBWindowRenderer(){
	
      };
      
      void XCBWindowRenderer::render(std::map<uuid_t,std::unique_ptr<DrawCommandBase>>&  commands , uuid_t  window_uuid){
	std::cout << "starting render" << std::endl;
	path_stack_t path_stack{};
	path_stack.push({0,window_uuid});

	auto bitmap = std::unique_ptr<uint8_t[]>(new uint8_t[this->size_.width * this->size_.height*4]); 
	this->clear(bitmap.get(),this->size_ );
	
	while(path_stack.size()>=1){
	  auto [child_pos, uuid] = path_stack.top( );
	  DrawCommandBase* next_item =  commands.at(uuid).get();
	  switch (next_item->command_type()){
	  case CommandType::node:
	    {
	      auto command = dynamic_cast<NodeCommand*>(next_item);
	      if ( command->children_.size()> child_pos){
		path_stack.push({0,command->children_[child_pos]});
	      } else {
		path_stack.pop();
		path_stack.top().first+=1;
	      }
	    };
	    break;
	  case CommandType::window_node:
	    {
	      auto command = dynamic_cast<WindowNodeCommand*>(next_item);
	      assert(path_stack.size() == 1);
	      if ( command->children_.size()> child_pos){
		path_stack.push({0,command->children_[child_pos]});
	      } else {
		path_stack.pop();
	      }
	    };
	    break;
	  case CommandType::draw_color:
	    this->render_command(
		dynamic_cast<DrawColorCommand*>(next_item),
		bitmap.get(),
		this->size_);
	    path_stack.pop();
	    path_stack.top().first+=1;
	    break;
	  case CommandType::draw_text:
	    this->render_command(
		dynamic_cast<DrawTextCommand*>(next_item),
		bitmap.get(),
		this->size_);
	    path_stack.pop();
	    path_stack.top().first+=1;
	    break;
	  default:
	    throw std::runtime_error( "unknown type");
	  };
	};
	xcb_image_t* image = xcb_image_create_from_bitmap_data(bitmap.get(), this->size_.width, this->size_.height);
	if(!image )
	  throw std::runtime_error("couldn't create image");
	image->format = XCB_IMAGE_FORMAT_XY_BITMAP;
	xcb_image_t * final_image = xcb_image_native(
	    this->connection_,
	    image,
	    1);
	xcb_pixmap_t  framebuffer= xcb_generate_id(this->connection_);
	xcb_create_pixmap(
	    this->connection_,
	    this->screen_->root_depth,
	    framebuffer,
	    this->window_,
	    this->size_.width,
	    this->size_.height);
	xcb_gcontext_t pixcontext = xcb_generate_id(this->connection_);
	uint32_t pixmask = XCB_GC_FOREGROUND|XCB_GC_BACKGROUND;
	uint32_t pixvalue[2];
	pixvalue[0] = this->screen_->black_pixel;
	pixvalue[1] = this->screen_->white_pixel;
	xcb_create_gc(
	    this->connection_,
	    pixcontext,
	    this->window_,
	    pixmask,
	    pixvalue);
	
	xcb_image_put(this->connection_, this->window_, pixcontext,final_image, 0,0,0);

	xcb_image_destroy(final_image);
	xcb_free_gc(this->connection_, pixcontext);
	xcb_map_window(this->connection_, this->window_);
      };

      void XCBWindowRenderer::clear(uint8_t *bitmap , widgets::Size size){
        for (int i=0; i<size.width*size.height*4; i++){
	  *(bitmap+i)=0xff;

	} ;
      };
      void XCBWindowRenderer::render_command(DrawColorCommand* command,uint8_t* bitmap, widgets::Size size){
	std::cout << "setting color"  << std::endl;;
	for(int i=0; i<size.width*size.height; i+=4 ){
	  auto ptr = bitmap+i;
	  auto color =command->color_;
	  ptr[0] = color.red;
	  ptr[1] = color.green;
	  ptr[2] = color.blue;
	  ptr[3] = color.alpha;
	};
      };
      void XCBWindowRenderer::render_command(DrawTextCommand* ,uint8_t*, widgets::Size){
      };


      
      XCBRenderer::XCBRenderer(){
	int screen_num;
	this->connection_ = xcb_connect(nullptr, &screen_num );
      };
      
      bool XCBRenderer::handle_events()  {
	xcb_generic_event_t * event;
	while( (event = xcb_wait_for_event(this->connection_))){
	  switch(event->response_type & ~0x80){
	  case XCB_EXPOSE:{
	    this->render();
	    xcb_flush(this->connection_ );
	    break;
	  };
	  default:{
	    break;
	  };
	  };
	  free(event);
	};
	return true;
      };

      void XCBRenderer::render(){

	if (not this->window_renderer_) {
	  this->window_renderer_ = std::make_unique<XCBWindowRenderer>( XCBWindowRenderer::create(this->connection_,Size{100,200}));
	};
	auto  root_command = [&](){
	  auto const&  it = this->commands_->find(this->root_);
	  auto const & [key, root_command] = *it;
	  assert(root_command->command_type() == CommandType::root_node);
	
	  return dynamic_cast<RootNodeCommand*>( root_command.get() );}();
	assert(root_command->children_.size() == 1);
	
	this->window_renderer_->render(*this->commands_, root_command->children_[0]);
      };
      void XCBRenderer::update_commands(
	  std::map<uuid_t,std::unique_ptr<DrawCommandBase>>* commands ,
	  uuid_t  root){
	using namespace std::chrono_literals;
	this->commands_ =  commands;
	this->root_ = root;
      };
      
      XCBRenderer::~XCBRenderer(){
	xcb_disconnect( this->connection_);

      };

    }// xcb
  }//backend
}// fluxpp
