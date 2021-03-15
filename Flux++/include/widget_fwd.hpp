#ifndef FLUXPP_WIDGET_FWD_HPP
#define FLUXPP_WIDGET_FWD_HPP
#include <boost/uuid/uuid.hpp>

namespace fluxpp{
  namespace visitors{
    class RenderVisitor;
  }

  using uuid_t = boost::uuids::uuid;
  namespace widgets{
    
    struct WidgetReturnContainer;
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
  }
  // BaseWidget
  namespace widgets{
    class BaseWidget {
    public:
      virtual std::vector<const std::string*> get_subscriptions()const=0;
	virtual uuid_t accept(visitors::RenderVisitor& visitor,
			      std::unique_ptr<BaseWidget>,
			      uuid_t parent_uuid )=0;
    };
  }// widgets
  
  namespace widgets{
    namespace application{
      class ApplicationBase {
      public:
	virtual std::vector<const std::string*> get_subscriptions()const=0;
	virtual void accept(visitors::RenderVisitor& visitor )=0;
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
      };
    }// screen
  }// widgets

  
}
#endif // FLUXPP_WIDGET_FWD_HPP
