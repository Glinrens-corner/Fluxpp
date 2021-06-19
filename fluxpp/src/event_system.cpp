#include "event_system.hpp"
#include "error.hpp"
#include "meta.hpp"
#include <iostream>
#include <cassert>

namespace fluxpp{


  namespace event_system{
    GlobalStringMapper::id_t GlobalStringMapper::map_string(std::string name){
      if (this->is_sealed_){
        throw fluxpp::error::sealed_access_exception();
      };
      {
        auto it =this->id_by_string_.find(name);
        if(it != this->id_by_string_.end()){
          return it->second;
        };
      };
      id_t new_id = this->generator_.get_id();
      auto [it, inserted] = this->string_by_id_
        .insert({new_id,
              std::move(name)});
      assert(inserted);
      std::string_view name_view{it->second} ;
      this->id_by_string_.insert({
          name_view,
            new_id});
      return new_id;
    };

    GlobalStringMapper & GlobalStringMapper::get_instance(){
      static GlobalStringMapper instance;
      return instance;
    };
    
    std::optional<GlobalStringMapper::id_t> GlobalStringMapper::id_by_string(const std::string&name)const{
      auto it =  this->id_by_string_.find(name);
      if ( it == this->id_by_string_.end()){
        return {};
      } else {
        return it->second;
      };
    };

    std::optional<std::reference_wrapper< const std::string>> GlobalStringMapper::string_by_id( GlobalStringMapper::id_t id)const{
      auto it = this->string_by_id_.find(id);
      if(it == this->string_by_id_.end()){
        return {};
      } else {
        return std::cref(it->second);
      };
    };

    LocalStringMapper GlobalStringMapper::create_local_mapper() const{
      this->is_sealed_ = true;
      return LocalStringMapper(this );
    };

    LocalStringMapper::LocalStringMapper(
        const GlobalStringMapper* global_mapper)
      :global_mapper_(global_mapper)
      ,generator_(global_mapper->generator_){
      assert(this->global_mapper_->is_sealed());
    };

    LocalStringMapper::~LocalStringMapper(){
    };
    
    LocalStringMapperInterface LocalStringMapper::get_interface(){
      this->lock();
      return LocalStringMapperInterface(this);
    };
    
    ConstLocalStringMapperInterface LocalStringMapper::get_interface()const{
      this->lock();
      return ConstLocalStringMapperInterface(this);
    };

    auto  ConstLocalStringMapperInterface::id_by_string(const std::string& name)const->std::optional<id_t>{
      return this->mapper_->id_by_string(name);
    };

    auto  LocalStringMapperInterface::id_by_string(const std::string& name)const->std::optional<id_t>{
      return this->mapper_->id_by_string(name);
    };


    std::optional<std::reference_wrapper<const std::string>> ConstLocalStringMapperInterface::string_by_id(id_t id)const{
      return this->mapper_->string_by_id(id);
    };

    std::optional<std::reference_wrapper<const std::string>> LocalStringMapperInterface::string_by_id(id_t id)const{
      return this->mapper_->string_by_id(id);
    };

    LocalStringMapperInterface::id_t LocalStringMapperInterface::map_string( std::string name){
      return this->mapper_->map_string( name);
    };

    ConstLocalStringMapperInterface::~ConstLocalStringMapperInterface( ){
      if(this->mapper_){
        this->mapper_->unlock();
      };
    };
    
    LocalStringMapperInterface::~LocalStringMapperInterface( ){
      if(this->mapper_){
        this->mapper_->unlock();
      };
    };
    
    auto  LocalStringMapper::id_by_string(const std::string& name)const->std::optional<id_t>{
      auto global_val = this->global_mapper_->id_by_string(name);
      if(global_val ){
        return global_val;
      } else {
        auto it = this->id_by_string_.find(name);
        if(it== this->id_by_string_.end()){
          return {};
        } else {
          return it->second;
        };
      };
    };
    
    std::optional<std::reference_wrapper<const std::string>> LocalStringMapper::string_by_id(id_t id )const{
      auto global_val = this->global_mapper_->string_by_id(id);
      if(global_val){
        return global_val;
      }else {
        auto it = this->string_by_id_.find(id);
        if(it == this->string_by_id_.end()){
          return {};
        } else {
          return std::cref(it->second);
        };
      };
    };

    auto LocalStringMapper::map_string(std::string name )->id_t{
      auto preset_val = this->id_by_string( name);
      if (preset_val.has_value()){
        return preset_val.value();
      }else {
        return this->map_string_core(name);
      };
    };
    
     auto LocalStringMapper::map_string_core(std::string name)->id_t{
      id_t new_id = this->generator_.get_id();
      auto [it, inserted] = this->string_by_id_
        .insert({new_id,
              std::move(name)});
      assert(inserted);
      std::string_view name_view{it->second} ;
      this->id_by_string_.insert({
          name_view,
            new_id});
      return new_id;
    };


    
  }// event_system

  namespace event_system {
    void Portal::dispatch_events_core(event_queue_t::iterator event_it){
      using deallocator_t = fluxpp::event_system::detail::PortalAllocator::Deallocator<AbstractEvent>;
      bool erase_from_event = false;
      std::visit(fluxpp::meta::overload{
          [this,&erase_from_event]
            (std::unique_ptr<AbstractEvent>&event,
             std::function<void(AbstractEvent*)>& dispatcher   ){
            dispatcher(event.get());
            erase_from_event = true;
          },
            [this,&erase_from_event]
              (std::unique_ptr<AbstractEvent,deallocator_t>&event,
               std::function<void(AbstractEvent*)>& dispatcher ){
              dispatcher(event.get());
              erase_from_event = true;
              
            },
              [this,&erase_from_event, &event_it]
                (std::unique_ptr<AbstractEvent,deallocator_t>&event,
                     std::pair<
                     std::function<bool(AbstractEvent*)>,
                     std::function<void(ranges::any_view<AbstractEvent*, ranges::category::forward >&)>
                     >& dispatcher ){
                if (dispatcher.first(event.get() )){
                  erase_from_event = true;

                  ranges::any_view<AbstractEvent*, ranges::category::forward> transformed_range = ranges::subrange(event_it, this->event_queue_.end() )
                      |ranges::views::transform([](std::variant<
                                            std::unique_ptr<AbstractEvent>,
                                            std::unique_ptr<AbstractEvent,deallocator_t>>& event)->AbstractEvent*{
                                           AbstractEvent * return_ptr = nullptr;
                                           std::visit(fluxpp::meta::overload{
                                               [&return_ptr](std::unique_ptr<AbstractEvent>& event ){return_ptr = event.get(); },
                                                 [&return_ptr](std::unique_ptr<AbstractEvent,deallocator_t>& event){return_ptr = event.get();}

                                             }
                                             ,event   );
                                           assert(return_ptr); 
                                           return return_ptr;
                                                });
                } else {
                  // handle the events not yet.
                  erase_from_event = false;
                  
                };
              },
                [this, event_it,&erase_from_event](std::unique_ptr<AbstractEvent>&event,
                            std::pair<std::function<bool(AbstractEvent*)>,
                  std::function<void(ranges::any_view<AbstractEvent*, ranges::category::forward >&)>
                            >& dispatcher){

                                       if (dispatcher.first(event.get() )){
                  erase_from_event = true;

                  ranges::any_view<AbstractEvent*, ranges::category::forward> transformed_range = ranges::subrange(event_it, this->event_queue_.end() )
                      |ranges::views::transform([](std::variant<
                                            std::unique_ptr<AbstractEvent>,
                                            std::unique_ptr<AbstractEvent,deallocator_t>>& event)->AbstractEvent*{
                                           AbstractEvent * return_ptr = nullptr;
                                           std::visit(fluxpp::meta::overload{
                                               [&return_ptr](std::unique_ptr<AbstractEvent>& event ){return_ptr = event.get(); },
                                                 [&return_ptr](std::unique_ptr<AbstractEvent,deallocator_t>& event){return_ptr = event.get();}

                                             }
                                             ,event   );
                                           assert(return_ptr); 
                                           return return_ptr;
                                                });
                } else {
                  // handle the events not yet.
                  erase_from_event = false;
                  
                };
                                       
                     }
          },*event_it, this->dispatcher_);
    };
    void Portal::set_dispatcher(std::function<void( AbstractEvent*)> dispatcher){
      this->dispatcher_ = dispatcher;
    };
    void Portal::set_buffered_dispatcher(std::function<bool(AbstractEvent* )>checker,
                                         std::function<void(ranges::any_view< AbstractEvent* , ranges::category::forward>& ) > dispatcher ){
      this->dispatcher_ = std::pair<std::function<bool(AbstractEvent* )>, std::function<void(ranges::any_view< AbstractEvent* , ranges::category::forward>& ) > >
        (checker,dispatcher);
    };

    Port EventSystem::get_port(){
      return Port(this);
    };

    Portal& EventSystem::get_portal(const PathSegment& segment) {
      using string_id_type = fluxpp::id::Id<StringIdTag>;
      const string_id_type* id = std::get_if<string_id_type>(&segment.content_ );
      if (id){
        return this->portals_.at(*id);
      } else {
        throw "Error all Portals need to be named";
      };
    };
    
  }// event_system
}// fluxpp
