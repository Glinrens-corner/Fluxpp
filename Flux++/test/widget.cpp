#include <doctest/doctest.h>
#include <mem_comparable_closure.hpp>
#include "widget.hpp"
#include "gui_event.hpp"


using namespace mem_comparable_closure;

using namespace fluxpp::widgets::screen;

using namespace fluxpp::widgets;

using namespace fluxpp::widgets::window;
using fluxpp::events::ButtonPressEvent;
using fluxpp::events::ButtonReleaseEvent;

using fluxpp::widgets::builtin::ColorWidget;
using fluxpp::widgets::builtin::Color;
using fluxpp::widgets::builtin::TextWidget;
using fluxpp::widgets::AppEvent;
using fluxpp::widgets::AppEventContainer;

Widget<SubscribeTo<bool>,
       ListenFor<ButtonPressEvent, ButtonReleaseEvent>
       > button = WidgetBuilder{}
  .with_filters(Filter<bool>("state/button"))
  .with_render_lambda([](bool clicked){
	if ( clicked) {
	  return make_widget_return_container(Size{.width=300, .height=150},
					      ColorWidget(Color::white ).at(0,0),
					      TextWidget("Click Me").at(0,0)
					      );
	} else {
	  return make_widget_return_container(Size{.width=300, .height=150},
					      ColorWidget(Color::black ).at(0,0),
					      TextWidget("Click Me").at(0,0)
					      );

	};
    })
  .for_events<ButtonPressEvent, ButtonReleaseEvent>()
  .build_with_event_handling_lambdas([](ButtonPressEvent event)->AppEventContainer
				 {return AppEvent("state/button");},

				 [] (ButtonReleaseEvent event )->AppEventContainer
				 {return AppEvent("state/button");}
				 );
/*
Window<> window =WindowBuilder{}
  .without_filter()
  .with_render_function([]( ){ return make_window_return_container( button); })
  .without_event_handler()
  .build();

Screen<> screen =ScreenBuilder{}
  .without_filter()
  .with_render_function([]( ){ return make_screen_return_container( window); })
  .without_event_handler()
  .build();
*/
TEST_CASE("widget"){
  using fluxpp::widgets::application::Application;
  using namespace fluxpp::widgets ;
};

