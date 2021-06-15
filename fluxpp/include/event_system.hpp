#ifndef FLUXPP_EVENT_SYSTEM_HPP
#define FLUXPP_EVENT_SYSTEM_HPP
#include <map>
#include <string>
#include <memory>
#include <string_view>
#include <optional>
#include <functional>
#include <cassert>
#include <variant>
#include <mutex>
#include <list>
#include "id.hpp"


namespace fluxpp {
  namespace event_system{
    struct StringIdTag{};

    class LocalStringMapper;
    
    class GlobalStringMapper{
    private:
      friend LocalStringMapper;
    public:
      using id_t = fluxpp::id::Id<StringIdTag>;
    private:
      GlobalStringMapper(){};
    public:
      GlobalStringMapper(const GlobalStringMapper &) = delete;
      void operator=(const GlobalStringMapper&) = delete; 
    public:
      static  GlobalStringMapper& get_instance();
      
      LocalStringMapper create_local_mapper( )const;
      
    public:
      id_t map_string(std::string name);
      
      bool is_sealed()const{return this->is_sealed_;};
      
    private: // only the local mapper has access to these.
      std::optional<id_t> id_by_string(const std::string& name)const;
      std::optional<std::reference_wrapper<const std::string>> string_by_id(id_t id )const;
    private:
      fluxpp::id::SequentialIdGenerator<StringIdTag> generator_{};
      std::map<id_t, std::string> string_by_id_{};
      std::map<std::string_view, id_t> id_by_string_{};
      mutable bool is_sealed_ = false;
    };

    class ConstLocalStringMapperInterface;
    class LocalStringMapperInterface;
    
    class LocalStringMapper{
    private:
      friend class ConstLocalStringMapperInterface;
      friend class LocalStringMapperInterface;
    public:
      using id_t = fluxpp::id::Id<StringIdTag>;
    public:
      LocalStringMapper(const GlobalStringMapper* global_mapper)
        :global_mapper_(global_mapper)
        ,generator_(global_mapper->generator_){
        assert(this->global_mapper_->is_sealed());
      };
      
      LocalStringMapperInterface get_interface(); 

      ConstLocalStringMapperInterface get_interface()const;
    private:
      std::optional<id_t> id_by_string(const std::string& )const;
      
      std::optional<std::reference_wrapper<const std::string>> string_by_id(id_t)const;

      id_t map_string( std::string);

      
    private:
      id_t map_string_core( std::string);

      void lock()const{this->lock_.lock(); };
      
      void unlock()const{this->lock_.unlock(); };
      
    private:
      const GlobalStringMapper* global_mapper_; 
      fluxpp::id::SequentialIdGenerator<StringIdTag> generator_{};
      std::map<id_t, std::string> string_by_id_{};
      std::map<std::string_view, id_t> id_by_string_{};
      mutable std::mutex  lock_{};
    };

    class LocalStringMapperInterface{
    private:
      friend class LocalStringMapper;
    public:
      using id_t = LocalStringMapper::id_t;
    private:
      LocalStringMapperInterface(LocalStringMapper* mapper):mapper_(mapper){};
    public:
      LocalStringMapperInterface():mapper_(nullptr){};

      LocalStringMapperInterface(const LocalStringMapperInterface& ) = delete;

      LocalStringMapperInterface(LocalStringMapperInterface&& other ){
        std::swap(this->mapper_, other.mapper_);
      };
      
      LocalStringMapperInterface& operator=(const LocalStringMapperInterface& ) = delete;
      
      LocalStringMapperInterface& operator=(LocalStringMapperInterface&& other){
        std::swap(this->mapper_, other.mapper_);
      };
      
      ~LocalStringMapperInterface(){
        if(this->mapper_){
          this->mapper_->unlock();
        };
      };
    public:
      std::optional<id_t> id_by_string(const std::string& )const;
      
      std::optional<std::reference_wrapper<const std::string>> string_by_id(id_t)const;

      id_t map_string(std::string);
      
    private:
      LocalStringMapper * mapper_;
    };
    
    class ConstLocalStringMapperInterface{
    private:
      friend class LocalStringMapper;
    public:
      using id_t = LocalStringMapper::id_t;
    private:
      ConstLocalStringMapperInterface(const LocalStringMapper* mapper):mapper_(mapper){};
    public:
      
      ConstLocalStringMapperInterface():mapper_(nullptr){};
      
      ConstLocalStringMapperInterface(const ConstLocalStringMapperInterface& ) = delete;
      
      ConstLocalStringMapperInterface(ConstLocalStringMapperInterface&& other ){
        std::swap(this->mapper_, other.mapper_);
      };
      
      ConstLocalStringMapperInterface& operator=(const ConstLocalStringMapperInterface& ) = delete;
      
      ConstLocalStringMapperInterface& operator=(ConstLocalStringMapperInterface&& other){
        std::swap(this->mapper_, other.mapper_);
      };
      
      ~ConstLocalStringMapperInterface(){
        if(this->mapper_){
          this->mapper_->unlock();
        };
      };
    public:
      std::optional<id_t> id_by_string(const std::string& )const;
      
      std::optional<std::reference_wrapper<const std::string>> string_by_id(id_t)const;
           
    private:
      const LocalStringMapper * mapper_;
    };

  } // event_system

  namespace event_system{
    class PathSegment{

      // TODO make it private:
    public:
      std::variant<
      fluxpp::id::Id<StringIdTag>,
      fluxpp::id::id_base_type> content_;
    };
    

    class Path{
      // TODO make it private:
    public:
      std::vector<PathSegment> content_;

    };

    
  } // event_system
  namespace event_system{
    class EventDispatchVisitor{
    public:
      template< class event_t>
      void dispatch_generic(event_t & event) {}; 

      template< class event_t>
      void dispatch_2DPlane(event_t & event) {}; 

    };
    
    class AbstractEvent{
    public:
      virtual void accept(EventDispatchVisitor& visitor) = 0;
    };

    template <class data_t>
    class DataEvent: public AbstractEvent{
    public:
      DataEvent(data_t data)
        :content_(std::move(data)){};
      void accept(EventDispatchVisitor& visitor){
        visitor.dispatch_generic(*this);
      };
    public:
      data_t content_ {};
    };
    class EventSystem;
    class Port;

    namespace detail{

      // void function does nothing.
      inline void void_function(AbstractEvent *){};

      class PortalAllocator{
      private:
        static PortalAllocator create(){
          return PortalAllocator();
        };
      public:
        PortalAllocator(){};

        template <class to_allocate_t>
        void* allocate(){
          return std::malloc(sizeof(to_allocate_t));
        };
        
        void deallocate( void * pointer){
          std::free(pointer);
        };
      };


    };
    
    class Portal{
      friend class Port;
    private:
      static constexpr bool sequential_mode{ true};
    private:
      template<class T>
      class Deallocator{
      public:
        Deallocator(detail::PortalAllocator*allocator )
          :allocator_(allocator){};
        
        void operator()(T *to_deallocate){
          to_deallocate->~T();
          this->allocator_->deallocate(to_deallocate);
        };
 
      private:
        detail::PortalAllocator* allocator_;
      };
    private:
      void dispatch_events_core();

    public:
      template<class event_t>
      void dispatch_event(Path path, event_t  event);

      void set_dispatcher(std::function<void( AbstractEvent*)> dispatcher ){
        this->dispatcher_ = dispatcher;
      };
    public:
      detail::PortalAllocator allocator_{};
      std::mutex lock{};
      std::list<
        std::variant<
          std::unique_ptr<AbstractEvent>,
          std::unique_ptr<AbstractEvent,Deallocator<AbstractEvent>>
    > > event_queue_ {};
      std::function<void(AbstractEvent*)> dispatcher_{detail::void_function};
    };

    template<class event_t>
    void Portal::dispatch_event(Path path, event_t  event){
      if (sequential_mode){
        this->event_queue_.push_front(
            std::unique_ptr<
            AbstractEvent,
            Deallocator<AbstractEvent>
            >(
                new ( this->allocator_.allocate<event_t>() ) event_t(std::move(event)),
                Deallocator<AbstractEvent>(&this->allocator_)
            ));
        
        this->dispatch_events_core();
      } else {
        assert(false);
      };
    };
    
    template<>
    inline void Portal::dispatch_event<std::unique_ptr<AbstractEvent>>(
        Path path,
        std::unique_ptr<AbstractEvent> event){
      if (sequential_mode){
        this->event_queue_.push_front(std::move(event));
        this->dispatch_events_core();
      } else {
        assert(false);
      };
    };

    
    class EventSystem{
    private:
      using string_id_t = fluxpp::id::Id<StringIdTag>; 
    public:
      using string_id_type = string_id_t;
      EventSystem(std::map<string_id_t, Portal> portals,
                  LocalStringMapper* string_mapper)
        :portals_(std::move(portals))
        ,string_mapper_(string_mapper){};
    public:

      Port get_port( );
  
      Portal& get_portal(const PathSegment& segment);

      static EventSystem  create(
          std::vector<string_id_t> portal_ids ,
          LocalStringMapper* string_mapper){
        std::map<string_id_t, Portal > portals{};
        for ( string_id_t id : portal_ids){
          auto [_, inserted ] =portals.try_emplace(id);
          if (not inserted){
            throw "douplicate portal Id";
          };
        };
        return EventSystem( std::move(portals), string_mapper );
      };

    private:
      std::map<string_id_t,Portal > portals_;
      LocalStringMapper* string_mapper_;
    };

    class Port {
    public:
      Port(EventSystem* event_system)
        :event_system_(event_system){};

      template<class event_t>
      void dispatch_event(Path target_path,event_t event);
      

    private:
      EventSystem* event_system_;
    };

    template <class event_t>
    void Port::dispatch_event(Path target_path, event_t event){
      static_assert(std::is_base_of<AbstractEvent, event_t>::value,"an event must be derived from AbstractEvent" ); 
        Portal& dispatch_portal = this
          ->event_system_
          ->get_portal(target_path.content_[0]);
        dispatch_portal
                      .dispatch_event(target_path, event);      
    };
    
    template<>
    inline void Port::dispatch_event<std::unique_ptr<AbstractEvent>>(Path target_path, std::unique_ptr<AbstractEvent> event){
      Portal& dispatch_portal = this
        ->event_system_
        ->get_portal(target_path.content_[0]);
      dispatch_portal
        .dispatch_event(target_path, std::move(event));
    };

  }// event_system
} // fluxpp

#endif // FLUXPP_EVENT_SYSTEM_HPP
