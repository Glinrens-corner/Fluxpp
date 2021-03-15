#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <map>
#include <set>
#include <vector>
#include <limits>
#include "ui.hpp"
#include "widget_node.hpp"

namespace fluxpp {


  //TODO it is probably better to specialize WidgetNode for mulitple widget classes
  //    (per specific SettingsClass)

  
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
    struct RenderIFCHolder{
      state::SynchronousStateInterface* state_sifc;
      backend::AsynchronousBackendInterfaceBase * backend_aifc;
      RenderTreeData * old_tree;
      RenderTreeData * new_tree;
      std::vector<backend::DrawCommandBase>* commands;
    };

    void RenderVisitor::visit_application() {
      this->render_ifcs->old_tree->application_at(this->visiting).accept(*this);
    };
    
    void  RenderVisitor::process_container(WidgetReturnContainer&& container ){
      auto widget_data = container.extract_data();
      std::vector<events::Coordinate> next_positions = widget_data.positions;
      /*uuid_t new_uuid =  
	this
	->add_node(
	WidgetNode(
	parent_=this->current_render_id,
	std::unique_ptr<BaseSettings>(
	new wigets::WidgetData(std::move(widget_data)))
	)
	);
	this->update_child(this->current_render_id,new_uuid);*/
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
    // visitors::RenderVisitor(&render_ifcs, this->tree_.root()).visit_application();
      
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
  
  void WidgetNode::accept(visitors::RenderVisitor& visitor){
    return this->widget_->accept( visitor);
  };
  void ApplicationNode::accept(visitors::RenderVisitor& visitor){
    return this->widget_->accept( visitor);
  };
  void WindowNode::accept(visitors::RenderVisitor& visitor){
    return this->widget_->accept( visitor);
  };
  void ScreenNode::accept(visitors::RenderVisitor& visitor){
    return this->widget_->accept( visitor);
  };
  RenderTree::~RenderTree(){
    if (this->impl){
      delete this->impl;
    };
  }
  
} // fluxpp
