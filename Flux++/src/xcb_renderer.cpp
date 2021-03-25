#include "backend/xcb_renderer.hpp"
#include <xcb/render.h>
#include <xcb/xcb_image.h>

#include <hb.h>
#include <hb-ft.h>
#include <thread>
#include <stdexcept>
#include <chrono>
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stack>
#include "widget.hpp"
#include "gui_event.hpp"
namespace {
  FT_Library ft_library;
}

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
 	}else {
	  throw std::exception();
	};
	if(FT_Init_FreeType(&ft_library)){
	  throw std::runtime_error("could not initiate freetype");
	};
	xcb_window_t win = xcb_generate_id(connection);
	uint32_t mask=XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	uint32_t values[2];
	values[0] = screen->white_pixel;
	values[1]= XCB_EVENT_MASK_EXPOSURE
	  |XCB_EVENT_MASK_BUTTON_PRESS
	  |XCB_EVENT_MASK_STRUCTURE_NOTIFY;
       
	xcb_create_window(
	    connection,
	    XCB_COPY_FROM_PARENT,
	    win,
	    screen->root,
	    0, 0,
	    size.width, size.height,
	    10,
	    XCB_WINDOW_CLASS_INPUT_OUTPUT,
	    screen->root_visual,
	    mask, values);
	const uint32_t config_values [] = {200,100 };
	xcb_configure_window(
	    connection,
	    win,
	    XCB_CONFIG_WINDOW_X |XCB_CONFIG_WINDOW_Y,
	    config_values) ;
	xcb_map_window(connection, win);
	xcb_flush(connection);
	return {win,screen,connection , size};
      };

      XCBWindowRenderer::~XCBWindowRenderer(){
	
      };
      
      void XCBWindowRenderer::render(std::map<uuid_t,std::unique_ptr<DrawCommandBase>>&  commands , uuid_t  window_uuid){
	path_stack_t path_stack{};
	path_stack.push({0,window_uuid});

	uint8_t* bitmap = new uint8_t[this->size_.width * this->size_.height*4]; 
	this->clear(bitmap,this->size_ );
	
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
		bitmap,
		this->size_);
	    path_stack.pop();
	    path_stack.top().first+=1;
	    break;
	  case CommandType::draw_text:
	    this->render_command(
		dynamic_cast<DrawTextCommand*>(next_item),
		bitmap,
		this->size_);
	    path_stack.pop();
	    path_stack.top().first+=1;
	    break;
	  default:
	    throw std::runtime_error( "unknown type");
	  };
	};
	xcb_image_t* image =xcb_image_create_native(
	    this->connection_,
	    this->size_.width,
	    this->size_.height,
	    XCB_IMAGE_FORMAT_Z_PIXMAP,
	    this->screen_->root_depth,
	    bitmap,
	    this->size_.width * this->size_.height*4,
	    bitmap
	);
	if(!image )
	  throw std::runtime_error("couldn't create image");
       
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
	
	xcb_image_put(this->connection_, this->window_, pixcontext,image, 0,0,0);

	xcb_image_destroy(image);
	xcb_free_gc(this->connection_, pixcontext);
	xcb_map_window(this->connection_, this->window_);
      };

      void XCBWindowRenderer::clear(uint8_t *bitmap , widgets::Size size){
        for (int i=0; i<size.width*size.height*4; i++){
	  *(bitmap+i)=0xff;

	} ;
      };
      void XCBWindowRenderer::render_command(DrawColorCommand* command,uint8_t* bitmap, widgets::Size size){
	auto color =command->color_;
	for(int i=0; i<size.width*size.height*4; i+=4 ){
	  auto ptr = bitmap+i;
	    
	  ptr[0] = color.blue;
	  ptr[1] = color.green;
	  ptr[2] = color.red;
	};
      };

      namespace detail{
	void draw_glyph(
	    uint8_t * bitmap,Size  bmsize,
	    FT_Face  face,
	    std::size_t bmstart_x,
	    std::size_t bmstart_y
	) {
	  if (not face->glyph->format==FT_GLYPH_FORMAT_BITMAP){
	  };
	  FT_Render_Glyph(face->glyph,
			  FT_RENDER_MODE_NORMAL);
	  auto glyphbm = &face->glyph->bitmap;
	  std::size_t render_width = std::min<unsigned long>(glyphbm->width, bmsize.width - bmstart_y );
	  std::size_t render_height = std::min<unsigned long>(glyphbm->rows, bmsize.height - bmstart_x );
	 
	  
	  for ( int i = 0; i<render_height;i++){
	    auto gl_rowptr =  glyphbm->buffer + i*glyphbm->pitch;
	    auto bm_rowptr =  bitmap + (i+bmstart_y) *4*bmsize.width;
	    for (int j = 0; j<render_width; j++){
	      (bm_rowptr+4*(j+bmstart_x))[0] = *(gl_rowptr+j);
	      (bm_rowptr+4*(j+bmstart_x))[1] =*(gl_rowptr+j);
	      (bm_rowptr+4*(j+bmstart_x))[2] =*(gl_rowptr+j);
	      (bm_rowptr+4*(j+bmstart_x))[3] = 0xff;
		
	    };
	  };
	};

      };
      
      void XCBWindowRenderer::render_command(DrawTextCommand* command,uint8_t* bitmap, widgets::Size bitmap_size){
	FT_Face ft_face;
	FT_Error ft_error;
	ft_error = FT_New_Face(
	    ft_library,
	    "/usr/share/fonts/truetype/LiberationSansNarrow-Regular.ttf",
	    0,
	    &ft_face);
	if( ft_error){
	  return ;
	};
	const char* text = "asdfg";
	hb_buffer_t *buf = hb_buffer_create();
	if (! hb_buffer_allocation_successful( buf)) {
	  throw std::runtime_error( "buffer creation failed");
	};
	hb_buffer_add_utf8(buf, text, -1, 0,-1  );
	
	FT_Set_Char_Size(ft_face,0,32*64,96,96);
	hb_font_t * hb_font; 
        hb_font = hb_ft_font_create(ft_face, nullptr);

	hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
	hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
	hb_buffer_set_language(buf, hb_language_from_string("en",-1));
	
	
	if (not hb_shape_full(hb_font,buf,nullptr, 0, nullptr)){
	  throw std::runtime_error( "shaping failed" );
	};
	unsigned int glyph_count;
	hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos( buf, &glyph_count);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions( buf, &glyph_count);

	std::size_t  cursor_x = 0;
	std::size_t  cursor_y = 16;
	for(unsigned int iglyph = 0; iglyph< glyph_count; iglyph++){
	  hb_codepoint_t glyphid = glyph_info[iglyph].codepoint;
	  if(FT_Load_Glyph(ft_face, glyph_info[iglyph].codepoint, FT_LOAD_DEFAULT)) {
	    throw std::runtime_error("failed to load glyph");
	  };
	       
	  detail::draw_glyph(
	      bitmap, bitmap_size,
	      ft_face,
	      cursor_x+glyph_pos[iglyph].x_offset,
	      cursor_y+glyph_pos[iglyph].y_offset
	  );
	  cursor_x += glyph_pos[iglyph].x_advance/64;
	  cursor_y += glyph_pos[iglyph].y_advance/64;
	};
      };


      
      XCBRenderer::XCBRenderer(){
	int screen_num;
	this->connection_ = xcb_connect(nullptr, &screen_num );
      };
      
      bool XCBRenderer::handle_events(RenderTree* render_tree)  {
	this->commands_.update_commands(
	    render_tree->get_synchronous_interface().extract_draw_commands()
	);
	
	this->render();
	xcb_flush(this->connection_ );
	
	xcb_generic_event_t * event;
	while( (event = xcb_wait_for_event(this->connection_))){
	  switch(event->response_type & ~0x80){
	  case XCB_EXPOSE:{
	    auto  expose_event = reinterpret_cast<xcb_expose_event_t *>( event); 
	    this->commands_.update_commands(
	        render_tree->get_synchronous_interface().extract_draw_commands()
	    );
	    this->render();
	    xcb_flush(this->connection_ );
	    break;
	  };
	  case XCB_BUTTON_PRESS:{
	    auto  button_event = reinterpret_cast<xcb_button_press_event_t *>( event); 
	    render_tree->get_synchronous_interface()
	      .dispatch_event(events::ButtonPressEvent{
		button_event->sequence,
		  Coordinate{.x=button_event->event_x,
		    .y=button_event->event_y},
		  button_event->state,
		  button_event->detail
	      });
	    this->commands_.update_commands(
	        render_tree->get_synchronous_interface().extract_draw_commands()
	    );
	    this->render();
	    break;
	  };
	  case XCB_CONFIGURE_NOTIFY:{
	   auto configure_notify_event =reinterpret_cast<xcb_configure_notify_event_t *>( event);
	   if (configure_notify_event->x != 200){
	     
	     const uint32_t config_values [] = {200,100 };
	     xcb_configure_window(
		 this->connection_,
		 this->window_renderer_->window(),
		 XCB_CONFIG_WINDOW_X |XCB_CONFIG_WINDOW_Y,
		 config_values) ;

		 };
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
	  auto const& [key, root_command]  = *this->commands_.commands().find(
	      this->commands_.root_uuid()
	  );
	  assert(root_command->command_type() == CommandType::root_node);
	
	  return dynamic_cast<RootNodeCommand*>( root_command.get() );}();
	assert(root_command->children_.size() == 1);
	
	this->window_renderer_->render(this->commands_.commands(), root_command->children_[0]);
      };
      
      void XCBRenderer::update_commands(
	  std::map<uuid_t,std::unique_ptr<DrawCommandBase>>* commands ,
	  uuid_t  root){
      };
      
      XCBRenderer::~XCBRenderer(){
	xcb_disconnect( this->connection_);

      };

    }// xcb
  }//backend
}// fluxpp
