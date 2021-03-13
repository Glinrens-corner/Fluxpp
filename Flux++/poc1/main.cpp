#include <mem_comparable_closure.hpp>
#include "widget.hpp"
#include "gui_event.hpp"
#include "ui.hpp"
#include "backend/xcb_backend.hpp"

using fluxpp::Ui;
using fluxpp::backend::XCBBackend;

using namespace mem_comparable_closure;

using namespace fluxpp::widgets::screen;
using namespace fluxpp::widgets::application;

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

Window<SubscribeTo<>, ListenFor<>> mywindow =WindowBuilder{}
  .without_filters()
  .with_render_lambda([]( ){ return make_window_return_container( Size{300,400},button); })
  .build_without_event_handlers();

Screen<SubscribeTo<>, ListenFor<>> myscreen =ScreenBuilder{}
  .without_filters()
  .with_render_lambda([]( ){ return make_screen_return_container(ScreenSettings{},mywindow); })
  .build_without_event_handlers();

Application<SubscribeTo<>, ListenFor<>> myapp =ApplicationBuilder{}
  .without_filters()
  .with_render_lambda([]( ){ return make_application_return_container(myscreen); })
  .build_without_event_handlers();

int main() {
  using namespace fluxpp;
  using widgets::AppEvent;
  auto backend = XCBBackend::create() ;
  Ui mygui= Ui::create(&backend, myapp);
  mygui.add_state_slice("state/button", state::StateSlice<bool>(true,
								[](bool state, const AppEvent& event  ){
								  return std::make_pair( not state , std::vector<AppEvent>{});
								}
								)
			);
};
