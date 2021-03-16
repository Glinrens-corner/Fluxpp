#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <map>
#include <set>
#include <vector>
#include <limits>
#include "ui.hpp"
#include "widget_node.hpp"

namespace fluxpp {
  
  class RenderTreeData{
  public:
      
    RenderTreeData(){};
      
    RenderTreeData(std::unique_ptr<widgets::application::ApplicationBase> application){
      this->insert(
	  ApplicationNode(
	      std::move(application),
	      widgets::application::ApplicationSettings{}
	  )
      );
    };
    
    ApplicationNode& application_at(uuid_t uuid){
      return this->application.at(uuid);
    };
    
    ScreenNode& screen_at(uuid_t uuid){
      return this->screens.at(uuid);
    };
    
    WindowNode& window_at(uuid_t uuid){
      return this->windows.at(uuid);
    };
    
    WidgetNode& widget_at(uuid_t uuid){
      return this->widget_tree.at(uuid);
    };

    template<class widget_node_t>
    uuid_t insert(widget_node_t new_widget){
      uuid_t uuid = this->gen_();
      auto [it,success ]=this->base_insert(uuid,std::move(new_widget));
      if (!success) throw std::exception();
      for (const std::string* subscription :it->second.get_subscriptions() ){
	this->subscribed_to.try_emplace(*subscription);
	this->subscribed_to[*subscription].insert(uuid);
	  
      };
      return uuid ;
    };
    ApplicationNode& root(){
      return this->application.begin()->second;
    };

  private:
    decltype(auto) base_insert(uuid_t uuid, ApplicationNode application){
      return this->application.insert({uuid,std::move(application)});
    };
    decltype(auto) base_insert(uuid_t uuid, ScreenNode screen){
      return this->screens.insert({uuid,std::move(screen)});
    };
    decltype(auto) base_insert(uuid_t uuid, WindowNode window){
      return this->windows.insert({uuid,std::move(window)});
    };
    decltype(auto) base_insert(uuid_t uuid, WidgetNode widget){
      return this->widget_tree.insert({uuid,std::move(widget)});
    };
  private:
    std::map<uuid_t, ApplicationNode> application{};
    std::map<uuid_t, ScreenNode> screens{};
    std::map<uuid_t, WindowNode> windows{};
    std::map<uuid_t, WidgetNode> widget_tree{};
    boost::uuids::random_generator gen_{};
    std::map<std::string, std::set<uuid_t>> subscribed_to{};
  };

  class RenderTree::RenderTreeImpl{
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
    
    void prepare_render(bool rerender_all);
    
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

  void RenderTree::RenderTreeImpl::prepare_render(bool rerender_all){
    auto state_ifc = this->state_->get_synchronous_interface();
      
    auto command_creator = this->backend_->get_asynchronous_interface();
      

    std::set<uuid_t> rerender_widgets{};
    std::set<std::string> updated_slices = state_ifc.get_updated_slices();

    std::vector<backend::DrawCommandBase> commands{ };
    RenderTreeData new_tree{};
    auto render_ifcs = visitors::RenderIFCHolder{
      .state_sifc=&state_ifc,
      .backend_aifc=command_creator.get(),
      .old_tree = &this->tree_,
      .new_tree=&new_tree,
      .commands=&commands };
    visitors::RenderVisitor(&render_ifcs, uuid_t{}).visit(&(this->tree_.root().widget()));
    
    std::swap(this->tree_, new_tree);
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
  
  void RenderTree::prepare_render(bool rerender_all){
    this->impl->prepare_render(rerender_all);
  };
  
  RenderTree::~RenderTree(){
    if (this->impl){
      delete this->impl;
    };
  }
  
} // fluxpp
