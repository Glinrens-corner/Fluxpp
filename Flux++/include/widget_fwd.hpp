#ifndef FLUXPP_WIDGET_FWD_HPP
#define FLUXPP_WIDGET_FWD_HPP
namespace fluxpp{
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
}
#endif // FLUXPP_WIDGET_FWD_HPP
