#include <boost/uuid/uuid.hpp>
#include <map>
#include <set>
#include <vector>
#include <limits>
#include <iostream>
#include "ui.hpp"
#include "widget_node.hpp"
#include "command_visitor.hpp"
#include "render_data.hpp"


namespace fluxpp {

  RenderTreeData::RenderTreeData(std::unique_ptr<widgets::application::ApplicationBase> application){
    this->insert(
	ApplicationNode(
	    std::move(application),
	    widgets::application::ApplicationSettings{}
	)
    );
  };
  
  ApplicationNode& RenderTreeData::application_at(uuid_t uuid){
    return this->application.at(uuid);
  };
    ScreenNode& RenderTreeData::screen_at(uuid_t uuid){
      return this->screens.at(uuid);
    };
    const ScreenNode& RenderTreeData::screen_at(uuid_t uuid)const{
      return this->screens.at(uuid);
    };

    WindowNode& RenderTreeData::window_at(uuid_t uuid){
      return this->windows.at(uuid);
    };
    const WindowNode& RenderTreeData::window_at(uuid_t uuid)const{
      return this->windows.at(uuid);
    };
    
    WidgetNode& RenderTreeData::widget_at(uuid_t uuid){
      return this->widget_tree.at(uuid);
    };
    const WidgetNode& RenderTreeData::widget_at(uuid_t uuid)const{
      return this->widget_tree.at(uuid);
    };
    ApplicationNode& RenderTreeData::root(){
      return this->application.begin()->second;
    };
    const ApplicationNode& RenderTreeData::root()const {
      return this->application.begin()->second;
    };


  


  class RenderTreeImpl{
  public:
    RenderTreeImpl(std::unique_ptr<widgets::application::ApplicationBase>&& application,
		   std::queue<AppEvent>*app_queue,
		   backend::BaseBackend* backend,
		   state::State* state_ptr)
      :app_queue_(app_queue)
      ,backend_(backend)
      ,state_(state_ptr)
      ,tree_(std::move(application)){
    };
    std::vector<std::unique_ptr<backend::DrawCommandBase>>extract_draw_commands(bool rerender_all);
    void prepare_render(bool rerender_all);
    
    std::vector<std::unique_ptr<backend::DrawCommandBase>> generate_commands(const RenderTreeData&);
  public:
    void lock(){};
    void unlock(){};
  private:
    std::queue<AppEvent>* app_queue_;
    backend::BaseBackend* backend_;
    state::State* state_;
    RenderTreeData tree_;
  };

  namespace visitors{

    void RenderVisitor::visit(widgets::application::ApplicationBase* application ) {
      application->accept(*this);
    };
    
    uuid_t RenderVisitor::visit( std::unique_ptr<widgets::screen::ScreenBase> screen, uuid_t parent_uuid){
      auto screen_ptr = screen.get();
      return screen_ptr->accept(*this, std::move(screen),  parent_uuid);
    };

    uuid_t RenderVisitor::visit( std::unique_ptr<widgets::window::WindowBase> window, uuid_t parent_uuid){
      auto window_ptr = window.get();
      return window_ptr->accept(*this, std::move(window),  parent_uuid);
    };
    
    uuid_t RenderVisitor::visit(
	std::unique_ptr<widgets::BaseWidget> widget,
	uuid_t parent_uuid){
      auto widget_ptr = widget.get();
      return widget_ptr->accept(*this, std::move(widget),  parent_uuid);
    };

    uuid_t RenderVisitor::render_widget(
	TextWidget& widget,
	std::unique_ptr<widgets::BaseWidget> base,
	uuid_t parent_uuid){
      auto widget_uuid =this->render_ifcs->new_tree->
	insert(
	    WidgetNode(
		parent_uuid,
		std::move(base),
	        widget.extract_data())
	);
      return widget_uuid;
    };
    
    uuid_t RenderVisitor::render_widget(
	ColorWidget& widget,
	std::unique_ptr<widgets::BaseWidget> base,
	uuid_t parent_uuid){
      
      auto widget_uuid =this->render_ifcs->new_tree->
	insert(
	    WidgetNode(
		parent_uuid,
		std::move(base),
	        widget.extract_data())
	);
	return widget_uuid;
      };
    
    uuid_t RenderVisitor::process_container(
	WidgetReturnContainer container ,
	std::unique_ptr<widgets::BaseWidget> widget,
	uuid_t parent_uuid){
      auto widget_data = container.extract_data();
      auto widget_ptr = widget.get();
      auto widget_uuid =this->render_ifcs->new_tree->
	insert(
	    WidgetNode(
		parent_uuid,
		std::move(widget),
	        widget_data)
	);
      std::vector<uuid_t> children{};
      children.reserve(container.widgets().size() );
      for(std::unique_ptr<widgets::BaseWidget>&  child :
	    container.extract_widgets( )){
	children.push_back(this->visit(std::move(child) ,widget_uuid));
      };
      this->render_ifcs->new_tree->widget_at(widget_uuid).children(std::move(children));
      return widget_uuid;
    };


    
    uuid_t  RenderVisitor::process_container(
        ScreenReturnContainer container ,
	std::unique_ptr<widgets::screen::ScreenBase> screen,
	uuid_t parent_uuid){
      auto screen_settings = container.extract_data();
      auto screen_ptr = screen.get();
      auto screen_uuid =this->render_ifcs->new_tree->
	insert(
	    ScreenNode(
		parent_uuid,
		std::move(screen),
		screen_settings)
	);
      
      std::vector<uuid_t> children{};
      children.reserve(container.widgets().size() );
      for(std::unique_ptr<widgets::window::WindowBase>&  window :
	    container.extract_windows( )){
	children.push_back(this->visit(std::move(window) ,screen_uuid));
      };
      this->render_ifcs->new_tree->screen_at(screen_uuid).children(std::move(children));
      return screen_uuid;
    };


    uuid_t  RenderVisitor::process_container(
        WindowReturnContainer container ,
	std::unique_ptr<widgets::window::WindowBase> window,
	uuid_t parent_uuid){
      auto window_settings = container.extract_data();
      auto window_ptr = window.get();
      auto window_uuid =this->render_ifcs->new_tree->
	insert(
	    WindowNode(
		parent_uuid,
		std::move(window),
	        window_settings)
	);
      
      std::vector<uuid_t> children{};
      children.reserve(container.widgets().size() );
      for(std::unique_ptr<widgets::BaseWidget>&  widget :
	    container.extract_widgets( )){
	children.push_back(this->visit(std::move(widget),window_uuid));
      };
      this->render_ifcs->new_tree->window_at(window_uuid).children(std::move(children));
      return window_uuid;
    };
    
    
    void  RenderVisitor::process_container(
	ApplicationReturnContainer container){
      widgets::application::ApplicationSettings application_data = container.extract_data();
      uuid_t app_uuid = this->render_ifcs->new_tree->
	insert(
	    ApplicationNode(
	        this->render_ifcs->old_tree->root().extract_widget(),
		std::move(application_data)
	    )
	);
      std::vector<uuid_t> children{};
      // TODO generate command
      children.reserve(container.widgets().size() );
      for (std::unique_ptr<widgets::screen::ScreenBase>&  screen : container.extract_widgets()  ){
	children.push_back(RenderVisitor(this->render_ifcs,uuid_t{}).visit(std::move(screen),app_uuid ));
      };
      this->render_ifcs->new_tree->application_at(app_uuid).children(std::move(children));
      return;
    };

  } // visitors

  std::vector<std::unique_ptr<backend::DrawCommandBase>> RenderTreeImpl::extract_draw_commands(bool rerender_all){
    auto state_ifc = this->state_->get_synchronous_interface();
      
    std::set<std::string> updated_slices = state_ifc.get_updated_slices();

    RenderTreeData new_tree{};
    auto render_ifcs = visitors::RenderIFCHolder{
      .state_sifc=&state_ifc,
      .old_tree = &this->tree_,
      .new_tree=&new_tree,
    };
    visitors::RenderVisitor(&render_ifcs, uuid_t{}).visit(&(this->tree_.root().widget()));
    std::vector<std::unique_ptr<backend::DrawCommandBase>>  commands
      = this->generate_commands(new_tree);
 
    std::swap(this->tree_, new_tree);
    return commands;
  };
  
  std::vector<std::unique_ptr<backend::DrawCommandBase>>  RenderTreeImpl::generate_commands( const RenderTreeData& tree){
    auto backend_aifc =  this->backend_->get_asynchronous_interface();
    auto& screen_uuids = tree.root().children();
    assert(screen_uuids.size() == 1);
    std::vector<std::unique_ptr<backend::DrawCommandBase>> ret{};
    visitors::CommandVisitor(backend_aifc.get(), &ret, &tree).visit_screen(*screen_uuids.begin() );
    return  ret;
  };
  RenderTree::RenderTree(
      std::unique_ptr<widgets::application::ApplicationBase> application,
      std::queue<AppEvent>* app_queue,
      backend::BaseBackend* backend,
      state::State* state_ptr)
    :impl(
	new RenderTreeImpl(
	    std::move(application),
	    app_queue,
	    backend,
	    state_ptr
	)
    ){};
  
  
  RenderTree::~RenderTree(){
    if (this->impl){
      delete this->impl;
    };
  }
  SynchronousRenderTreeInterface RenderTree::get_synchronous_interface(){
    this->impl->lock();
    return SynchronousRenderTreeInterface(this->impl);
  };

  std::vector<std::unique_ptr<backend::DrawCommandBase>> SynchronousRenderTreeInterface::extract_draw_commands(){
    return this->impl->extract_draw_commands(false);
  }
  SynchronousRenderTreeInterface::~SynchronousRenderTreeInterface(){
    if(this->impl){
      this->impl->unlock();
    };
  };
} // fluxpp
