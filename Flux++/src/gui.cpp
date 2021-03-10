#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <map>
#include <set>
#include <vector>
#include "gui.hpp"

namespace fluxpp {
  class WidgetNode{
  public:
    WidgetNode(std::unique_ptr<widgets::BaseWidget>&& widget):widget_(std::move(widget)){};
    std::vector<std::string*> get_subscriptions( ){
      return this->widget_->get_subscriptions();
    };
  private:
    std::unique_ptr<widgets::BaseWidget> widget_;
  };
  using boost::uuids::uuid;

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
      auto& [it, success] =this->widgets.insert({this->application_uuid, WidgetNode(std::move( application)) } );
      if(not success){
	// wtf can't insert into empty map?
	throw std::exeption();
      };
      
    };

    uuid insert(uuid ,WidgetNode widget) {
      for( auto & subscription : widget.get_subscriptions()){
	this->subscribed_to.try_emplace(subscription);
	this->subscribed_to[*subscription].insert(uuid);
      };
      this->widgets_.insert({uuid, std::move(widget)});
      return uuid;
    };
  private:
    boost::uuids::random_generator gen{};
    uuid application_uuid;
    std::queue<AppEvent>* app_queue_;
    backend::BaseBackend* backend_;
    state::State* state_;
    std::map<uuid, WidgetNode>widgets_{};
    std::map<std::string, std::set<uuid>> subscribed_to{} ;
  };

  RenderTree::RenderTree(std::unique_ptr<widgets::BaseWidget>&& application,
			 std::queue<AppEvent>* app_queue,
			 backend::BaseBackend* backend,
			 state::State* state_ptr)
    :impl( new RenderTreeImpl(std::move(application),
			      app_queue,
			      backend,
			      state_ptr) )
  {};
  
  void prepare_render(){ this->impl->prepare_render();};
  RenderTree::~RenderTree(){
    if (this->impl){
      delete this->impl;
    };
  }


}
