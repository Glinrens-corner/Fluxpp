#ifndef FLUXPP_WIDGET_FWD_HPP
#define FLUXPP_WIDGET_FWD_HPP
#include "uuid.hpp"
#include "visitor_fwd.hpp"
#include "app_event.hpp"
#include <vector>
#include <mem_comparable_closure.hpp>
#include "backend/base_backend.hpp"
namespace fluxpp{

  namespace widgets{
    class WidgetReturnContainer;
    template<class Arg_t>
    struct Filter;
    namespace screen {
      struct ScreenReturnContainer;
    }
    namespace window {
      struct WindowReturnContainer;
    }
    namespace application{
      struct ApplicationReturnContainer;
    }
    namespace builtin{
      class ColorWidget;
      class TextWidget;
    }
  }// widgets
  
  // BaseWidget
  namespace widgets{
    class BaseWidget {
    public:
      virtual std::vector<const std::string*> get_subscriptions()const=0;
      virtual std::unique_ptr<backend::DrawCommandBase> accept(
	  uuid_t parent_uuid,
	  uuid_t widget_uuid,
	  std::vector<uuid_t> children,
	  visitors::CommandVisitor&
      )const =0;
      virtual uuid_t accept(visitors::RenderVisitor& visitor,
			    std::unique_ptr<BaseWidget>,
			    uuid_t parent_uuid )=0;
      
      virtual void accept(visitors::DispatchVisitor& visitor )=0;
    };
    template<class subscriptions_t, class listened_for_t>
    struct  Widget;//: public BaseWidget;
  }// widgets
  
  namespace widgets{
    namespace application{
      class ApplicationBase {
      public:
	virtual std::vector<const std::string*> get_subscriptions()const=0;
	virtual void accept(visitors::RenderVisitor& visitor )=0;
	virtual void accept(visitors::DispatchVisitor& visitor )=0;
      };
    }// application
  }// widgets


  namespace widgets{
    namespace window{
      class WindowBase {
      public:
	virtual std::vector<const std::string*> get_subscriptions()const=0;
	virtual uuid_t accept(visitors::RenderVisitor& visitor,
			      std::unique_ptr<WindowBase>,
			      uuid_t parent_uuid )=0;
	virtual void accept(visitors::DispatchVisitor& visitor )=0;
      };
    }// application
  }// widgets

  namespace widgets{
    enum class WidgetSuperclass{
      application,
      screen,
      window,
      widget
    };
      
    namespace screen{
      class ScreenBase {
      public:
	virtual std::vector<const std::string*> get_subscriptions()const=0;
	virtual uuid_t accept(visitors::RenderVisitor& visitor,
			      std::unique_ptr<ScreenBase>,
			      uuid_t parent_uuid )=0;
	virtual void accept(visitors::DispatchVisitor& visitor)=0;
      };
    }// screen
  }// widgets
  // EventHandler
  namespace widgets{
    using mem_comparable_closure::ClosureMaker;
    using mem_comparable_closure::Function;
    
    namespace detail{
      // a little helper class to handle special EventHandler constructors
      //
      template<
	class app_event_t,
	class gui_event_t,
	class T>
      struct event_handler_helper{
	static constexpr decltype(auto)convert( T&& fn) {
	  return ClosureMaker<AppEventContainer, const gui_event_t &>
	    ::make(fn)
	    .as_fun();
	};
      };
      
      template<
	class app_event_t,
	class gui_event_t>
      struct event_handler_helper<
	app_event_t,
	gui_event_t,
	Function<AppEventContainer,const gui_event_t &> >{
	
	static constexpr decltype(auto) convert(
	    Function<AppEventContainer,const gui_event_t &>&& fn
	) {
	  return fn;
	};
	
      };
    }//detail
    
    template< class app_event_t, class gui_event_t>
    class EventHandler {
    public:
      template<class T>
      explicit EventHandler(T fn ):
	function_(
	    detail
	    ::event_handler_helper<app_event_t, gui_event_t, T>
	    ::convert(std::move(fn))) { };
		   
      EventHandler(const EventHandler<app_event_t,const gui_event_t &>& other ):
	function_(other.function_.copy()) { };

      AppEventContainer operator( ) ( const gui_event_t& event ){
	return this->function_(event);
      };
      
    public:
      Function<AppEventContainer,const gui_event_t &> function_;
    };
  }// widgets

  
}
#endif // FLUXPP_WIDGET_FWD_HPP
