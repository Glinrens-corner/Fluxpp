#ifndef FLUXPP_WIDGET_FWD_HPP
#define FLUXPP_WIDGET_FWD_HPP

namespace fluxpp{
  namespace visitors{
    class RenderVisitor;
  }

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
      virtual void accept(visitors::RenderVisitor& visitor )=0;
    };
  }// widgets

}
#endif // FLUXPP_WIDGET_FWD_HPP
