#ifndef WIDGET_HPP
#define WIDGET_HPP
#include <string>
#include <tuple>
#include <array>
#include <mem_comparable_closure.hpp>
#include "gui_event.hpp"

namespace fluxpp{
  //using declarations
  // Size
  namespace widgets{
    using mem_comparable_closure::Function;
    using mem_comparable_closure::ClosureMaker;
    using std::tuple;
    using std::array;
    using events::Coordinate;

    struct Size{
      int16_t width;
      int16_t height;
    };
  };
  
  // AppEvent
  // AppEventContainer
  namespace widgets{
    struct AppEvent{
      AppEvent(std::string target):target(target){};
      std::string target;
    };
    

    struct AppEventContainer{
      AppEventContainer(AppEvent event):event(std::move(event)){};
      AppEvent event;
    };
  }
  
  // LocatedWidget
  namespace widgets{
    // LocatedWidget is a little holder intended
    // for a widget an its position
    template <class widget_t>
    struct LocatedWidget{
      LocatedWidget(widget_t widget, Coordinate coord ):
        coordinate(coord),
	widget(std::move(widget)){};
      Coordinate coordinate;
      widget_t widget;
    };
  }
  
  // Filter
  namespace widgets{
    template<class return_t>
    struct Filter {
      Filter( std::string target_):target(std::move(target_)){ };
      std::string target;
    };
  }

  // EventHandler
  namespace widgets{
    template< class app_event_t, class gui_event_t>
    class EventHandler {
    public:
      template<class T>
      EventHandler(T fn ):
	function_(ClosureMaker<AppEventContainer, gui_event_t>::make(fn).as_fun()) { };
      template<>
      EventHandler(Function<AppEventContainer,gui_event_t> function):
	function_(std::move(function)){};
    public:
      Function<AppEventContainer,gui_event_t> function_;
    };
  }

  // two metaprogramming helper classes
  namespace widgets{
    template<class ...T>
    struct SubscribeTo{
      template< template <class >typename C >
      using map = SubscribeTo<C<T>...>;
      template<template<class ...>typename C>
      using apply = C<T...>;
    };

    template<class ...T>
    struct ListenFor{
      template< template <class >typename C >
      using map = ListenFor<C<T>...>;
      template<template<class ...>typename C>
      using apply = C<T...>;
    };
  }

  // WidgetContainer
  namespace widgets{
    class WidgetReturnContainerBase{};

    template<class ...widget_ts>
    class WidgetReturnContainer : public WidgetReturnContainerBase{
      using widgets_tuple_t = std::tuple<widget_ts...> ;
    public:
      WidgetReturnContainer(Size size , widgets_tuple_t widgets)
	: size_(size),
	  widgets_(widgets) {};
    private:
      widgets_tuple_t widgets_;
      Size size_ ;
    };

    template <class ...Arg_ts>
    WidgetReturnContainerBase * make_widget_return_container( Size size, Arg_ts... args){
      return new WidgetReturnContainer<Arg_ts...>(size, std::make_tuple(args...));
    };

  }
  
  // Widget
  // WidgetBuilder 
  namespace widgets {
    template<class subscriptions_t, class listened_for_t>
    struct  Widget{
    private:
      template<class ...F>
      using to_function = Function<WidgetReturnContainerBase*, F...>;
      template <class T>
      using to_event_handler = EventHandler<AppEvent, T>;

      using filters_tuple_t = typename subscriptions_t
	::template map<Filter>
	::template apply<std::tuple>;
     
      using function_t = typename subscriptions_t
	::template apply<to_function> ;   
     
      using listener_tuple_t = typename listened_for_t
	::template map<to_event_handler>
	::template apply<std::tuple>;
    public:
      Widget(filters_tuple_t filters,
	     function_t render_function,
	     listener_tuple_t listeners) :
	filters_(std::move(filters)),
	render_function_ (std::move(render_function)),
	listeners_(std::move(listeners)){};
    private:
      filters_tuple_t filters_;
      function_t render_function_;
      listener_tuple_t listeners_;
    };

    
    struct WidgetBuilder{
    private:
      template<class ...F>
      using to_fn_ptr = WidgetReturnContainerBase*(*)(F... );
     
      template<class ...F>
      using to_closure_maker = ClosureMaker<WidgetReturnContainerBase*, F...>;
     
      template<class ...F>
      using to_function = Function<WidgetReturnContainerBase*, F...>;
     
      template <class T>
      using to_event_handler = EventHandler<AppEvent,T>;
    public:
      // OK, normally a Builder returns itself. We do this a bit differently here as the builder also collects the types of its arguments.
      // The builder has 4 main states.
      //   state 0 (WidgetBuilder) the sourrounding state
      //   state 1 when Filters are set
      //   state 2 when also the render function is set
      //   state 2.5 when the subscribed to EventTypes are set
      //   // state 3 when the event handler are set. we currently build directly.

      template <class subscriptions_t, class listened_for_t>
      struct WidgetBuilderState2_5;

      template < class subscriptions_t,  class ... listened_for_ts  >
      struct WidgetBuilderState2_5<subscriptions_t, ListenFor<listened_for_ts...>>{
      private:
	using listened_for_t = ListenFor<listened_for_ts...>;
       
	using filters_tuple_t = typename subscriptions_t
	  ::template map<Filter>
	  ::template apply<std::tuple>;
       
	using function_t = typename subscriptions_t
	  ::template apply<to_function> ;
      public:
	WidgetBuilderState2_5(filters_tuple_t filters,
			      function_t function)
	  :filters(filters),
	   render_function(std::move(function)){};
       
	template<class ...Arg_ts>
	decltype(auto) build_with_event_handling_lambdas(Arg_ts...args){

	  auto tpl =  std::make_tuple(EventHandler<AppEvent, listened_for_ts>(args ) ... );

	  return Widget<subscriptions_t, listened_for_t>(std::move(this->filters),

							 std::move(this->render_function),
							 std::move(tpl)
	  );
	};
	filters_tuple_t filters;
	function_t render_function;

      };

      // sub_t is for a SubscribeTo<...> class
      template <class sub_t>
      struct WidgetBuilderState2{
      private:
	using filters_tuple_t = typename sub_t
	  ::template map<Filter>
	  ::template apply<std::tuple>;
	using function_t = typename sub_t
	  ::template apply<to_function> ;
      public:
	WidgetBuilderState2(filters_tuple_t filters,
			    function_t function):
	  filters(filters),
	  render_function(std::move(function)){
	};

	template<class ...listened_for_ts>
	decltype(auto) for_events(){
	  using state2_5_builder_t = WidgetBuilderState2_5<sub_t, ListenFor<listened_for_ts...>>; 
	  return state2_5_builder_t(this->filters, std::move(this->render_function) );
	};

	filters_tuple_t filters;
	function_t render_function;
      };

      template <class sub_t>
      struct WidgetBuilderState1{
	using filter_tuple_t = typename sub_t
	  ::template map<Filter>::template apply<std::tuple>; 
	template<class T>
	decltype(auto) with_render_lambda(T fn){
	  using fn_ptr_t  =  typename sub_t
	    ::template apply< to_fn_ptr>;

	  // casting to a function pointer to make sure it is a non capturing lambda
	  fn_ptr_t fn_ptr = fn;
	  using closure_maker_t = typename sub_t
	    ::template apply<to_closure_maker>;
	  using function_t = typename sub_t
	    ::template apply<to_function>;
	 
	  return WidgetBuilderState2<sub_t>(
	      this->filters,
	      closure_maker_t::make(fn_ptr).as_fun()
	  );
	};

        filter_tuple_t filters;
      };

      template<class ... subscription_ts>
      decltype(auto) with_filters(Filter<subscription_ts>... filters){
	using follow_t = WidgetBuilderState1<SubscribeTo<subscription_ts...> >; 
	return follow_t{std::make_tuple(filters...)};
      };

      WidgetBuilderState1<SubscribeTo<>> without_filters(){
	return WidgetBuilderState1<SubscribeTo<>>{std::make_tuple()};
      };

    };
  }
  namespace widgets {



    


    namespace builtin{
      enum class Color{
	black,
	white
      };
      
      class ColorWidget{
      public:
	ColorWidget(Color color):color_(color){};
	LocatedWidget<ColorWidget> at(int16_t x, int16_t y) {
	  return LocatedWidget<ColorWidget>(*this, Coordinate{x,y}); };
      private:
	Color color_;
      };

      class TextWidget{
      public:
	TextWidget(std::string text):text_(std::move(text)){}
	LocatedWidget<TextWidget> at(int16_t x, int16_t y) {
	  return LocatedWidget<TextWidget>(*this, Coordinate{x,y}); };
      private:
	std::string text_;
      };
    };

  }
  namespace widgets{
    namespace screen {

    }
  }
  namespace widgets{

    namespace window {

    }
  }
  // application
  namespace widgets{
    namespace application{
      class ApplicationReturnContainerBase{};
      
      template<class ...widget_ts>
      class ApplicationReturnContainer : public ApplicationReturnContainerBase{
	using widgets_tuple_t = std::tuple<widget_ts...> ;
      public:
      ApplicationReturnContainer( widgets_tuple_t widgets)
	: widgets_(widgets) {};
    private:
	widgets_tuple_t widgets_;
    };

    template <class ...Arg_ts>
    ApplicationReturnContainerBase * make_application_return_container(  Arg_ts... args){
      return new ApplicationReturnContainer<Arg_ts...>( std::make_tuple(args...));
    };
    }
    
    namespace application{
      struct Application{};
    } //app
  }// widget

} // fluxpp





#endif
