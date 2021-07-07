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
#include <range/v3/all.hpp>
#include "id.hpp"
#include <fmt/format.h>

namespace fluxpp {
  namespace event_system{
    struct StringIdTag{};

    // I am calling this pattern sealing singleton:
    // I want to allow access to string mapping(string interning) in a global context
    // prior to entering main.
    // However singletons cause problems in multithreaded contexts.
    // the solution here is that after entering main
    //     the user has to create a LocalStringMapper
    //     that contains a reference to the global singleton but
    //     records all additional entries.
    //     from the time the local object is created the singleton cannot be modified anymore.
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
      GlobalStringMapper(GlobalStringMapper&& ) = default;
      GlobalStringMapper& operator=(const GlobalStringMapper&) = delete; 
      GlobalStringMapper& operator=(GlobalStringMapper&&)=default;
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
      LocalStringMapper(const GlobalStringMapper* global_mapper);
      ~LocalStringMapper();
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
        using std::swap;
        swap(this->mapper_, other.mapper_);
      };
      
      LocalStringMapperInterface& operator=(const LocalStringMapperInterface& ) = delete;
      
      LocalStringMapperInterface& operator=(LocalStringMapperInterface&& other){
        using std::swap;
        swap(this->mapper_, other.mapper_);
        return *this;
      };
      
      ~LocalStringMapperInterface();
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
        return *this;
      };
      
      ~ConstLocalStringMapperInterface();
    public:
      std::optional<id_t> id_by_string(const std::string& )const;
      
      std::optional<std::reference_wrapper<const std::string>> string_by_id(id_t)const;
           
    private:
      const LocalStringMapper * mapper_;
    };

  } // event_system


  //PathSegment and Path
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

  // EventDispatchVisitor
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
      virtual const Path& target()const = 0;
      virtual  std::string fmt()const; 
    };
    
    inline std::string AbstractEvent::fmt()const { return "...";};
    
    template <class data_t>
    class DataEvent: public AbstractEvent{
    public:
      DataEvent(Path target, data_t data)
        :content_(std::move(data))
        ,target_{std::move(target) }{};
      void accept(EventDispatchVisitor& visitor){
        visitor.dispatch_generic(*this);
      };
      const Path& target()const{return this->target_;};
    public:
      data_t content_ {};
      Path target_{{}};
    };

    class RenderRequestEvent{};
    
    class EventSystem;
    
    class Port;

    namespace detail{

      // a function that does absolutely nothing function does nothing.
      inline void void_function(AbstractEvent *){};

      class PortalAllocator{
      public:
        template<class allocated_t>
        class Deallocator{
        public:
          Deallocator(PortalAllocator*allocator);

          void operator()(allocated_t *to_deallocate);
        private:
          PortalAllocator* allocator_;
          
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

        template<class allocated_t>
        Deallocator<allocated_t> get_deallocator(){
          return Deallocator<allocated_t>(this);
        };

      };

      template<class allocated_t>
      PortalAllocator::Deallocator<allocated_t>::Deallocator(PortalAllocator *allocator )
        :allocator_(allocator){};

      template<class allocated_t>
      void PortalAllocator::Deallocator<allocated_t>::operator()(allocated_t* to_deallocate){
        to_deallocate->~allocated_t();
        this->allocator_->deallocate(to_deallocate);
      };
    };
    
    class Portal{
      friend class Port;
    private:
      using event_queue_t = std::list<
        std::variant<
          std::unique_ptr<AbstractEvent>,
          std::unique_ptr<AbstractEvent,detail::PortalAllocator::Deallocator<AbstractEvent>>
          > >;
      using dispatcher_t =std::variant<
        std::function<void(AbstractEvent*)> ,
        std::pair<std::function<bool(AbstractEvent*)>,
                  std::function<void(ranges::any_view<AbstractEvent*, ranges::category::forward >&)>
                  >
        >;
    private:
      static constexpr bool sequential_mode{ true};
 
    private:
      void dispatch_events_core(event_queue_t::iterator);

    public:
      template<class event_t>
      void dispatch_event(Path path, event_t  event);

      void set_dispatcher(std::function<void( AbstractEvent*)> dispatcher );
      
      void set_buffered_dispatcher(
          std::function<bool(AbstractEvent* )>,
          std::function<void(ranges::any_view<
                             AbstractEvent* ,
                             ranges::category::forward > & ) > );
    public:
      detail::PortalAllocator allocator_{};
      std::mutex lock{};
      event_queue_t event_queue_ {};
      dispatcher_t dispatcher_{detail::void_function};
    };

    template<class event_t>
    inline void Portal::dispatch_event(Path path, event_t  event){
      if (sequential_mode){
        this->event_queue_.push_front(
            std::unique_ptr<
            AbstractEvent,
            detail::PortalAllocator::Deallocator<AbstractEvent>
            >(
                new ( this->allocator_.allocate<event_t>() ) event_t(std::move(event)),
                detail::PortalAllocator::Deallocator<AbstractEvent>(&this->allocator_)
            ));
        
        this->dispatch_events_core(this->event_queue_.begin());
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
        this->dispatch_events_core(this->event_queue_.begin() );
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
template<> struct fmt::formatter<fluxpp::event_system::AbstractEvent > {
  constexpr auto parse(format_parse_context&ctx) ->decltype(ctx.begin()) {
    auto it = ctx.begin();
    auto end = ctx.end();
    for(; it!=end and *it!= '}'; ++it);

    return it;
  };

  template<typename FormatContext>
  auto format(const  fluxpp::event_system::AbstractEvent& event, FormatContext &ctx)->decltype(ctx.out()){
    return format_to(
        ctx.out(),
        "Event({} )",
        event.fmt());


  };

};
#endif // FLUXPP_EVENT_SYSTEM_HPP
