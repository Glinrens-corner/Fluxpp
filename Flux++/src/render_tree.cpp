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




  // WidgetNode is a wrapper around the widget that allows the render tree to store additional information.
  class WidgetNode{
  public:
    WidgetNode(std::unique_ptr<widgets::BaseWidget>&& widget)
      :widget_(std::move(widget)){};
    
    std::vector<const std::string*> get_subscriptions( ){
      return this->widget_->get_subscriptions();
    };
    
    //    decltype(auto) accept(RenderVisitor& visitor ){
    //      return this->widget_->accept( visitor);
    //    };
     
    const std::vector<uuid_t>& children()const {return this->children_;};
    
    void children(std::vector<uuid_t> vec ){
      this->children_ = std::move(vec);
    };
    
    uuid_t parent() const {return this->parent_;};
    
    void parent(uuid_t new_parent){this->parent_ =new_parent;};
    
  private:
    std::unique_ptr<widgets::BaseWidget> widget_;
    std::vector<uuid_t> children_ ;
    uuid_t parent_;
  };




  
  class RenderTreeData{
  public:
    RenderTreeData(WidgetNode root){
      this->root_uuid= this->insert(std::move(root));
    };
    
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
    uuid_t root_uuid;
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
    
    void prepare_render(bool rerender_all){
      auto state_ifc = this->state_->get_synchronous_interface();
      
      auto command_creator = this->backend_->get_asynchronous_interface();


      std::set<uuid_t> rerender_widgets{};
      std::set<std::string> updated_slices = state_ifc.get_updated_slices();
      /*      
      if (rerender_all){
	for ( const auto & [key,value ] : this->widgets_){
	  rerender_widgets.insert(key);
	}
      } else {
	for (const std::string & slice : updated_slices){
	  const auto & index_set = this->tree_.subscribed_to(slice);	  
	  rerender_widgets.insert(index_set.cbegin(), index_set.cend());
	};
	};*/
      //      this->render_widgets(command_creator.get(),&state_ifc,rerender_widgets );
    };
    
    /*
    void render_widgets(
        backend::AsynchronousBackendInterfaceBase * backend_aifc,
	state::SynchronousStateInterface* state_ifc,
	const std::set<uuid_t>& render_widget_uuids){

      if (render_widgets.size() ==0 ) return;
      
      std::map<uuid_t, WidgetNode> new_widgets{};
      
      while (render_widget_uuids.size() >0 ) {
	uuid_t next_render_widget_uuid = this->get_next_render_widget_uuid(const set<uuid_t>& render_widget_uuids);
      
	//TODO implement render visitor.
	auto [new_widgets, untouched_widgets, draw_commands] =
	  RenderVisitor(state_ifc, backend_aifc, &this->widgets_);
	.visit(uuid);
      };
      
      
    };
    */
    /*
    uuid_t get_next_render_widget_uuid(const set<uuid_t>& render_widget_uuids){
      uint16_t level = numeric_limits<uint16_t>::value;
      uuid_t next_uuid{};
      for(auto uuid : render_widget_uuids){
	if(this->widgets_.at(uuid).get_level() < level){
	  next_uuid = uuid;
	  level = this->widgets_.at(uuid).level();
	};
      };
      assert( level <numeric_limits<uint16_t>::value);
      return next_uuid;
    }
    uuid_t insert(uuid_t uuid ,WidgetNode widget) {
      for( auto & subscription : widget.get_subscriptions()){
	this->subscribed_to.try_emplace(*subscription);
	this->subscribed_to[*subscription].insert(uuid);
      };
      this->widgets_.insert({uuid, std::move(widget)});
      return uuid;
    };
    */
  private:
    std::queue<AppEvent>* app_queue_;
    backend::BaseBackend* backend_;
    state::State* state_;
    RenderTreeData tree_;
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
  
  RenderTree::~RenderTree(){
    if (this->impl){
      delete this->impl;
    };
  }
  
} // fluxpp
