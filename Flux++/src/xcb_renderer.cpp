#include "backend/xcb_renderer.hpp"
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

      
      XCBWindowRenderer XCBWindowRenderer::create(xcb_connection_t * connection) {
        xcb_screen_t*  screen = xcb_setup_roots_iterator(
	    xcb_get_setup(connection)).data;
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
			  150, 150,
			  10,
			  XCB_WINDOW_CLASS_INPUT_OUTPUT,
			  screen->root_visual,
			  mask, values);
	xcb_map_window(connection, win);
	xcb_flush(connection);
	return {win,screen,connection };
      };

      XCBWindowRenderer::~XCBWindowRenderer(){
	
      };
      
      void XCBWindowRenderer::render(std::map<uuid_t,std::unique_ptr<DrawCommandBase>>&  commands , uuid_t  window_uuid){
	path_stack_t path_stack{};
	path_stack.push({0,window_uuid});
	xcb_gcontext_t gcontext = xcb_generate_id(this->connection_);
	uint32_t mask = XCB_GC_FOREGROUND|XCB_GC_GRAPHICS_EXPOSURES;
	uint32_t value[2];
	value[0] = this->screen_->black_pixel;
	value[1] = 0;
	xcb_create_gc(
	    this->connection_,
	    gcontext,
	    this->window_,
	    mask,
	    value);

	xcb_rectangle_t rectangle {0,0,150,150 };
	xcb_poly_fill_rectangle(
	    this->connection_,
	    this->window_,
	    gcontext,
	    1,
	    &rectangle
	);
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
	    path_stack.pop();
	    path_stack.top().first+=1;
	    break;
	  case CommandType::draw_text:
	    path_stack.pop();
	    path_stack.top().first+=1;
	    break;
	  default:
	    throw std::runtime_error( "unknown type");
	  };
	};

	
	xcb_map_window(this->connection_, this->window_);
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
	  this->window_renderer_ = std::make_unique<XCBWindowRenderer>( XCBWindowRenderer::create(this->connection_));
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
