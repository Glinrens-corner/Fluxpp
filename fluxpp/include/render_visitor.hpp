#ifndef FLUXPP_RENDER_VISITOR_HPP
#define FLUXPP_RENDER_VISITOR_HPP
namespace fluxpp{
  namespace event_system{
    class AbstractEvent;
  }// event_system
}// fluxpp
namespace fluxpp{
  namespace event_system{
    class AbstractEventDispatchVisitor{

    public:
      virtual void dispatch_generic_v(fluxpp::event_system::AbstractEvent* ,  const std::type_info& ) =0;
      virtual void dispatch_2d_point_v(fluxpp::event_system::AbstractEvent* ,  const std::type_info& ) =0;

    public:
      template<class event_t>
      void dispatch_generic(  event_t *);
      
      template<class event_t>
      void dispatch_2d_point(  event_t *);
      
    };

    template<class event_t>
    void  AbstractEventDispatchVisitor::dispatch_generic(event_t * event) {
      this->dispatch_generic_v(event, typeid( typename std::remove_cv<event_t>::type));
    };
    template<class event_t>
    void  AbstractEventDispatchVisitor::dispatch_2d_point(event_t * event) {
      this->dispatch_2d_point_v(event, typeid( typename std::remove_cv<event_t>::type));
    };
    
  } // event_system
}// fluxpp


#endif //FLUXPP_RENDER_VISITOR_HPP
