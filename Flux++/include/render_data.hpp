#ifndef FLUXPP_RENDER_DATA_HPP
#define FLUXPP_RENDER_DATA_HPP
#include "widget.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <map>

namespace fluxpp{
  class RenderTreeData{
  public:
      
    RenderTreeData(){};
    RenderTreeData(std::unique_ptr<widgets::application::ApplicationBase> application);
    ApplicationNode& application_at(uuid_t uuid);
    WindowNode& window_at(uuid_t uuid);
    const WindowNode& window_at(uuid_t uuid)const;
    ScreenNode& screen_at(uuid_t uuid);
    const ScreenNode& screen_at(uuid_t uuid)const;
    WidgetNode& widget_at(uuid_t uuid);
    const WidgetNode& widget_at(uuid_t uuid)const;

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
    ApplicationNode& root();
    const ApplicationNode& root()const;
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
  

}// fluxpp


#endif // FLUXPP_RENDER_DATA_HPP
