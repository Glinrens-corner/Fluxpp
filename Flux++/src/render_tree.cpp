#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <map>
#include <set>
#include <vector>
#include <limits>
#include "ui.hpp"

namespace fluxpp {
  // NOTE, we probably can work with generational indices as uuid.
  using uuid_t = boost::uuids::uuid;


  //TODO it is probably better to specialize WidgetNode for mulitple widget classes
  //    (per specific SettingsClass)

  // WidgetNode is a wrapper around the widget that allows the render tree to store additional information.
  class WidgetNode{
  public:
    WidgetNode(std::unique_ptr<widgets::BaseWidget>&& widget,
	       std::unique_ptr<widgets::BaseSettings>&& settings)
      :widget_(std::move(widget))
      ,settings_(std::move(settings)){};

    WidgetNode(uuid_t parent_uuid,
	       std::unique_ptr<widgets::BaseWidget>&& widget,
	       std::unique_ptr<widgets::BaseSettings>&& settings)
      :widget_(std::move(widget))
      ,settings_(std::move(settings))
      ,parent_(parent_uuid){};

    
    std::vector<const std::string*> get_subscriptions( ){
      return this->widget_->get_subscriptions();
    };


    void accept(visitors::RenderVisitor& visitor );
     
    const std::vector<uuid_t>& children()const {return this->children_;};
    
    bool update_child(uuid_t old, uuid_t updated ){
      for ( auto& child : this->children_){
	if (child == old){
	  child = updated;
	  return true;
	};
      };
      return false;
    };

    
    
    uuid_t parent() const {return this->parent_;};
    
    void parent(uuid_t new_parent){this->parent_ =new_parent;};
    
  private:
    std::unique_ptr<widgets::BaseSettings> settings_;
    std::unique_ptr<widgets::BaseWidget> widget_;
    std::vector<uuid_t> children_{} ;
    uuid_t parent_{};
  };    
    class RenderTreeData{
    public:
      
      RenderTreeData(){};
      
      RenderTreeData(std::unique_ptr<widgets::BaseWidget> application){
	this->root_uuid_= this->insert(WidgetNode(std::move(application), std::make_unique<widgets::BaseSettings>())  );
      };
      uuid_t insert_root( WidgetNode new_widget){
	this->root_uuid_ = this->insert(std::move(new_widget));
	return this->root_uuid_;
      }
      WidgetNode& at(uuid_t uuid){
	return this->widget_tree.at(uuid);
      }
      uuid_t root(){return this->root_uuid_; };
      
      uuid_t insert(WidgetNode new_widget){
	uuid_t uuid = this->gen_();
	auto [it,success ]=this->widget_tree.insert({uuid,std::move(new_widget)});
	if (!success) throw std::exception();
	for (const std::string* subscription :it->second.get_subscriptions() ){
	  this->subscribed_to.try_emplace(*subscription);
	  this->subscribed_to[*subscription].insert(uuid);
	  
	};
	return uuid ;
      };
      
    private:
      uuid_t root_uuid_;
      std::map<uuid_t, WidgetNode> widget_tree{};;
      boost::uuids::random_generator gen_{};
      std::map<std::string, std::set<uuid_t>> subscribed_to{};
    };
    


  class RenderTree::RenderTreeImpl{
  public:
    RenderTreeImpl(std::unique_ptr<widgets::BaseWidget>&& application,
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

    void RenderVisitor::visit() {
      this->render_ifcs->old_tree->at(this->visiting).accept(*this);

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
      visitors::RenderVisitor(&render_ifcs, this->tree_.root()).visit();
      
    };

  
  RenderTree::RenderTree(
      std::unique_ptr<widgets::BaseWidget>&& application,
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
  RenderTree::~RenderTree(){
    if (this->impl){
      delete this->impl;
    };
  }
  
} // fluxpp
