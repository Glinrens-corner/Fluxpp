#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <map>
#include <set>
#include <vector>
#include "ui.hpp"

namespace fluxpp {
  // NOTE, we probably can work with generational indices as uuid.
  using uuid_t = boost::uuids::uuid;
  class WidgetNode{
  public:
    WidgetNode(std::unique_ptr<widgets::BaseWidget>&& widget):widget_(std::move(widget)){};
    std::vector<const std::string*> get_subscriptions( ){
      return this->widget_->get_subscriptions();
    };
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

  class RenderTree::RenderTreeImpl{
  public:
    RenderTreeImpl(std::unique_ptr<widgets::BaseWidget>&& application,
		   std::queue<AppEvent>*app_queue,
		   backend::BaseBackend* backend,
		   state::State* state_ptr)
      :app_queue_(app_queue)
      ,backend_(backend)
      ,state_(state_ptr){

      this->application_uuid = gen();
      this->insert(this->application_uuid, WidgetNode(std::move( application)));
      
    };
    void prepare_render(bool rerender_all){
      auto state_ifc = this->state_->get_synchronous_interface();
      auto command_creator = this->backend_->get_asynchronous_interface();

      std::set<std::string> updated_slices = state_ifc.get_updated_slices();
      std::set<uuid_t> rerender_widgets{};
      if (rerender_all){
	for ( const auto & [key,value ] : this->widgets_){
	  rerender_widgets.insert(key);
	}
      } else {
	for (const auto & slice : updated_slices){
	  const auto & index_set = this->subscribed_to.at(slice);
	  rerender_widgets.insert(index_set.cbegin(), index_set.cend());
	};
      };
      this->render_widgets(command_creator.get(),&state_ifc,rerender_widgets );
      auto color_command = command_creator->get_draw_color_command();
    };

    void render_widgets(
        backend::AsynchronousBackendInterfaceBase *,
        fluxpp::state::SynchronousStateInterface*,
	const std::set<uuid_t>& rerender_widgets){
      
    };
    void delete_item (){
    };
    
    uuid_t insert(uuid_t uuid ,WidgetNode widget) {
      for( auto & subscription : widget.get_subscriptions()){
	this->subscribed_to.try_emplace(*subscription);
	this->subscribed_to[*subscription].insert(uuid);
      };
      this->widgets_.insert({uuid, std::move(widget)});
      return uuid;
    };
  private:
    boost::uuids::random_generator gen{};
    uuid_t application_uuid;
    std::queue<AppEvent>* app_queue_;
    backend::BaseBackend* backend_;
    state::State* state_;
    std::map<uuid_t, WidgetNode>widgets_{};
    std::map<std::string, std::set<uuid_t>> subscribed_to{} ;
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


}
