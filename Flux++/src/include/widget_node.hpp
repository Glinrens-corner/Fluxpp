#ifndef FLUXPP_WIDGET_NODE_HPP
#define FLUXPP_WIDGET_NODE_HPP

#include <boost/uuid/uuid.hpp>
#include "widget.hpp"


namespace fluxpp{
  // NOTE, we probably can work with generational indices as uuid.
  using uuid_t = boost::uuids::uuid;
  // WidgetNode is a wrapper around the widget that allows the render tree to store additional information.

  class ApplicationNode{
    using settings_t = widgets::application::ApplicationSettings;
    using base_t = widgets::application::ApplicationBase;
  public:
    ApplicationNode(std::unique_ptr<base_t> widget,
		     settings_t settings)
      :widget_(std::move(widget))
      ,settings_(std::move(settings)){};
    
    std::vector<const std::string*> get_subscriptions( ){
      return this->widget_->get_subscriptions();
    };

     
    const std::vector<uuid_t>& children()const {return this->children_;};
    void children(std::vector<uuid_t> new_children ) {
      this->children_= new_children;
    };

    bool update_child(uuid_t old, uuid_t updated ){
      for ( auto& child : this->children_){
	if (child == old){
	  child = updated;
	  return true;
	};
      };
      return false;
    };

    std::unique_ptr<base_t> extract_widget(){return std::move(this->widget_);};
    
    base_t& widget(){ return *(this->widget_);};
    
    const base_t& widget()const{ return *(this->widget_);};
  private:
    settings_t settings_;
    std::unique_ptr<base_t> widget_;
    std::vector<uuid_t> children_{} ;
  };

  class WindowNode{
    using base_t = widgets::window::WindowBase;
    using settings_t = widgets::window::WindowSettings;
  public:

    WindowNode(uuid_t parent_uuid,
	       std::unique_ptr<base_t> widget,
	       settings_t settings)
      :widget_(std::move(widget))
      ,settings_(std::move(settings))
      ,parent_(parent_uuid){};
    
    std::vector<const std::string*> get_subscriptions( ){
      return this->widget_->get_subscriptions();
    };

     
    const std::vector<uuid_t>& children()const {return this->children_;};
    void children(std::vector<uuid_t> new_children ) {
      this->children_= new_children;
    };

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

    base_t& widget(){ return *(this->widget_);};
    
    const base_t& widget()const{ return *(this->widget_);};
    
  private:
    settings_t settings_;
    std::unique_ptr<base_t> widget_;
    std::vector<uuid_t> children_{} ;
    uuid_t parent_;
  };

  class ScreenNode{
    using base_t = widgets::screen::ScreenBase;
    using settings_t = widgets::screen::ScreenSettings;
  public:

    ScreenNode(uuid_t parent_uuid,
	       std::unique_ptr<base_t> widget,
	       settings_t settings)
      :widget_(std::move(widget))
      ,settings_(std::move(settings))
      ,parent_(parent_uuid){};
    
    std::vector<const std::string*> get_subscriptions( ){
      return this->widget_->get_subscriptions();
    };

     
    const std::vector<uuid_t>& children()const {return this->children_;};
    void children(std::vector<uuid_t> new_children ) {
      this->children_= new_children;
    };
    
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

    base_t& widget(){ return *(this->widget_);};
    const base_t& widget()const{ return *(this->widget_);};
    
  private:
    settings_t settings_;
    std::unique_ptr<base_t> widget_;
    std::vector<uuid_t> children_{} ;
    uuid_t parent_;
  };

  class WidgetNode{
  public:
    WidgetNode(uuid_t parent_uuid,
	       std::unique_ptr<widgets::BaseWidget> widget,
	       widgets::WidgetData settings)
      :widget_(std::move(widget))
      ,settings_(std::move(settings))
      ,parent_(parent_uuid){};
    
    std::vector<const std::string*> get_subscriptions( ){
      return this->widget_->get_subscriptions();
    };

    const std::vector<uuid_t>& children()const {return this->children_;};
    void children(std::vector<uuid_t> new_children ) {
      this->children_= new_children;
    };

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
    
    widgets::BaseWidget& widget(){ return *(this->widget_);};
    const widgets::BaseWidget& widget()const{ return *(this->widget_);};
    
  private:
    widgets::WidgetData settings_;
    std::unique_ptr<widgets::BaseWidget> widget_;
    std::vector<uuid_t> children_{} ;
    uuid_t parent_{};
  };

}// fluxpp

#endif FLUXPP_WIDGET_NODE_HPP
