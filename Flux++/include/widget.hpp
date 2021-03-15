#ifndef FLUXPP_WIDGET_HPP
#define FLUXPP_WIDGET_HPP

#include <string>
#include <tuple>
#include <array>
#include <mem_comparable_closure.hpp>
#include "gui_event.hpp"
#include "app_event.hpp"
#include "render_visitor.hpp"

namespace fluxpp{
  //using declarations
  // Size
  namespace widgets{
    using visitors::RenderVisitor;
    using mem_comparable_closure::Function;
    using mem_comparable_closure::ClosureMaker;
    using std::tuple;
    using std::array;
    using events::Coordinate;

    struct Size{
      int32_t width;
      int32_t height;
    };
  };// widgets

  // BaseSettings
  namespace widgets{
    struct BaseSettings{};
  } // widgets

  
  // LocatedWidget
  namespace widgets{
    // LocatedWidget is a little holder intended
    // for a widget an its position
    template <class widget_t>
    struct LocatedWidget{
      LocatedWidget(widget_t widget, Coordinate coord ):
        coordinate(coord),
	widget(std::move(widget)){};

    public:
      Coordinate coordinate;
      widget_t widget;
    };
  }// widgets
  
  // Filter
  namespace widgets{
    template<class return_t>
    struct Filter {
      Filter( std::string target_):target(std::move(target_)){ };
      
      std::string target;
    };
  }// widgets

  // EventHandler
  namespace widgets{
    namespace detail{
      // a little helper class to handle special EventHandler constructors
      //
      template<
	class app_event_t,
	class gui_event_t,
	class T>
      struct event_handler_helper{
	static constexpr decltype(auto)convert( T&& fn) {
	  return ClosureMaker<AppEventContainer, gui_event_t>
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
	Function<AppEventContainer,gui_event_t> >{
	static constexpr decltype(auto) convert(
	    Function<AppEventContainer,gui_event_t>&& fn
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
		   
      EventHandler(const EventHandler<app_event_t,gui_event_t>& other ):
	function_(other.function_.copy()) { };
      
    public:
      Function<AppEventContainer,gui_event_t> function_;
    };
  }// widgets

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
  }// widgets

  // WidgetContainer
  namespace widgets{
    struct WidgetData: public BaseSettings{
    public:
      WidgetData(Size size, std::vector<Coordinate> positions)
	:size(size)
	,positions(positions){}

    public:
      Size size;
      std::vector<Coordinate> positions;
    };



    
    class WidgetReturnContainer {
      using widgets_vector_t = std::vector<std::unique_ptr<BaseWidget>>;
      
    public:
      WidgetReturnContainer(WidgetData data , widgets_vector_t widgets )
	: data_(data),
	  widgets_(std::move(widgets)) {};
      
      WidgetData extract_data(){return std::move(this->data_); }; 
    private:
      WidgetData data_ ;
      widgets_vector_t widgets_;
    };
    
    namespace detail{
      
      template<class ...Arg_ts>
      struct WidgetReturnContainerMaker;

      template<class ...Ts>
      struct WidgetReturnContainerMaker<Size,LocatedWidget<Ts>...>{
	static WidgetReturnContainer make(Size size, LocatedWidget<Ts>...widgets ){
	  std::vector<BaseWidget*> vec1{
	    static_cast<BaseWidget*>(new Ts{widgets.widget})...
	      };
	  
	  return WidgetReturnContainer(
	      WidgetData(size, std::vector<Coordinate>{ widgets.coordinate...}),std::vector<std::unique_ptr<BaseWidget>>(vec1.cbegin(),vec1.cend())
	  );
	}
      };
    }//detail
    
    template <class ...Arg_ts>
    WidgetReturnContainer make_widget_return_container(  Arg_ts... args){
       
      return detail::WidgetReturnContainerMaker<Arg_ts...>::make( std::move(args)...);
    };

  }// widgets
  
  // Widget
  // WidgetBuilder 
  namespace widgets {

    
    template<class subscriptions_t, class listened_for_t>
    struct  Widget: public BaseWidget{
    private:
      template<class ...F>
      using to_function = Function<WidgetReturnContainer, F...>;
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
      std::vector<const std::string*> get_subscriptions()const{
	std::vector<const std::string*> vec{};
	this->get_subscription<0>(this->filters_,vec);
	return vec;
      };
      
      void accept(RenderVisitor& visitor ){
      	visitor.render_widget(this->filters_, this->render_function_ );
      };
      
    private:
      template <int i, class ...tuple_ts>
      typename std::enable_if<(i<sizeof...(tuple_ts)) , void>::type
      get_subscription( const std::tuple<tuple_ts...>& filters,
			std::vector<const std::string*> vec )const{
	vec.push_back(&(std::get<i>(filters).target));
	this->get_subscription<i+1>(filters,vec);
      };

      template <int i, class ...tuple_ts>
      typename std::enable_if<not (i<sizeof...(tuple_ts)) , void>::type
      get_subscription( const std::tuple<tuple_ts...>& ,
			std::vector< const std::string*>  )const{
	
	
      };

      
      filters_tuple_t filters_;
      function_t render_function_;
      listener_tuple_t listeners_;
    };

    
    struct WidgetBuilder{
    private:
      template<class ...F>
      using to_fn_ptr = WidgetReturnContainer(*)(F... );
     
      template<class ...F>
      using to_closure_maker = ClosureMaker<WidgetReturnContainer, F...>;
     
      template<class ...F>
      using to_function = Function<WidgetReturnContainer, F...>;
     
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
  }// widgets

  // builtin
  namespace widgets {
    namespace builtin{
      enum class Color{
	black,
	white
      };
      
      class ColorWidget:public BaseWidget{
      public:
	ColorWidget(Color color):color_(color){};
	std::vector<const std::string*> get_subscriptions()const{
	  return {};
	};

	void accept(RenderVisitor& visitor){
	  visitor.render_widget(*this );
	};
	
	LocatedWidget<ColorWidget> at(int16_t x, int16_t y) {
	  return LocatedWidget<ColorWidget>(*this, Coordinate{x,y}); };
      private:
	Color color_;
      };

      class TextWidget:public BaseWidget{
      public:
	std::vector<const std::string*> get_subscriptions()const{
	  return {};
	}
	
	void accept(RenderVisitor& visitor){
	  visitor.render_widget(*this );
	};

	TextWidget(std::string text):text_(std::move(text)){}
	
	LocatedWidget<TextWidget> at(int16_t x, int16_t y) {
	  return LocatedWidget<TextWidget>(*this, Coordinate{x,y}); };
      private:
	std::string text_;
      };
    };
  } // widgets

  namespace widgets{
    namespace screen {
      
      struct ScreenSettings:public  BaseSettings{};
      

      class ScreenReturnContainer{
	using widgets_vector_t = std::vector<std::unique_ptr<window::WindowBase>> ;
      public:
	ScreenReturnContainer(ScreenSettings settings , widgets_vector_t widgets):settings_(settings)
										 ,widgets_(std::move(widgets)) {};
      private:
	widgets_vector_t widgets_;
	ScreenSettings settings_ ;
      };

       
    namespace detail{
      
      template<class ...Arg_ts>
      struct ScreenReturnContainerMaker;

      template<class ...Ts>
      struct ScreenReturnContainerMaker<ScreenSettings,Ts...>{
	static ScreenReturnContainer make(
	    ScreenSettings settings, Ts...windows){
	  std::vector<window::WindowBase*> vec1{
	    static_cast<window::WindowBase*>(new Ts{std::move(windows)})...
	      };
	  
	  return ScreenReturnContainer(
	      settings
	      ,std::vector<std::unique_ptr<window::WindowBase>>(vec1.cbegin(),vec1.cend())
	  );
	}
      };
    }//detail
    
    template <class ...Arg_ts>
    ScreenReturnContainer make_screen_return_container(  Arg_ts... args){
       
      return detail::ScreenReturnContainerMaker<Arg_ts...>::make( std::move(args)...);
    };


    

      
    template<class subscriptions_t, class listened_for_t>
    struct  Screen:public ScreenBase{
    private:
      template<class ...F>
      using to_function = Function<ScreenReturnContainer, F...>;
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
  
      Screen(filters_tuple_t filters,
	     function_t render_function,
	     listener_tuple_t listeners) :
	filters_(std::move(filters)),
	render_function_ (std::move(render_function)),
	listeners_(std::move(listeners)){};
      
      std::vector<const std::string*> get_subscriptions()const{
	return {};
      };
      
      void accept(RenderVisitor& visitor ){
      	visitor.render_widget(this->filters_, this->render_function_ );
      };


    private:
      filters_tuple_t filters_;
      function_t render_function_;
      listener_tuple_t listeners_;
    };

    
    struct ScreenBuilder{
    private:
      template<class ...F>
      using to_fn_ptr = ScreenReturnContainer(*)(F... );
     
      template<class ...F>
      using to_closure_maker = ClosureMaker<ScreenReturnContainer, F...>;
     
      template<class ...F>
      using to_function = Function<ScreenReturnContainer, F...>;
     
      template <class T>
      using to_event_handler = EventHandler<AppEvent,T>;
    public:
      // OK, normally a Builder returns itself. We do this a bit differently here as the builder also collects the types of its arguments.
      // The builder has 4 main states.
      //   state 0 (ScreenBuilder) the sourrounding state
      //   state 1 when Filters are set
      //   state 2 when also the render function is set
      //   state 2.5 when the subscribed to EventTypes are set
      //   // state 3 when the event handler are set. we currently build directly.

      template <class subscriptions_t, class listened_for_t>
      struct ScreenBuilderState2_5;

      template < class subscriptions_t,  class ... listened_for_ts  >
      struct ScreenBuilderState2_5<subscriptions_t, ListenFor<listened_for_ts...>>{
      private:
	using listened_for_t = ListenFor<listened_for_ts...>;
       
	using filters_tuple_t = typename subscriptions_t
	  ::template map<Filter>
	  ::template apply<std::tuple>;
       
	using function_t = typename subscriptions_t
	  ::template apply<to_function> ;
      public:
	ScreenBuilderState2_5(filters_tuple_t filters,
			      function_t function)
	  :filters(filters),
	   render_function(std::move(function)){};
       
	template<class ...Arg_ts>
	decltype(auto) build_with_event_handling_lambdas(Arg_ts...args){

	  auto tpl =  std::make_tuple(EventHandler<AppEvent, listened_for_ts>(args ) ... );

	  return Screen<subscriptions_t, listened_for_t>(std::move(this->filters),

							 std::move(this->render_function),
							 std::move(tpl)
	  );
	};
	filters_tuple_t filters;
	function_t render_function;

      };

      // sub_t is for a SubscribeTo<...> class
      template <class sub_t>
      struct ScreenBuilderState2{
      private:
	using filters_tuple_t = typename sub_t
	  ::template map<Filter>
	  ::template apply<std::tuple>;
	using function_t = typename sub_t
	  ::template apply<to_function> ;
      public:
	ScreenBuilderState2(filters_tuple_t filters,
			    function_t function):
	  filters(filters),
	  render_function(std::move(function)){
	};

	decltype(auto) build_without_event_handlers(){
	  return this->for_events<>().build_with_event_handling_lambdas();
	};

	template<class ...listened_for_ts>
	decltype(auto) for_events(){
	  using state2_5_builder_t = ScreenBuilderState2_5<sub_t, ListenFor<listened_for_ts...>>; 
	  return state2_5_builder_t(this->filters, std::move(this->render_function) );
	};

	
	filters_tuple_t filters;
	function_t render_function;
      };

      template <class sub_t>
      struct ScreenBuilderState1{
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
	 
	  return ScreenBuilderState2<sub_t>(
	      this->filters,
	      closure_maker_t::make(fn_ptr).as_fun()
	  );
	};

        filter_tuple_t filters;
      };

      template<class ... subscription_ts>
      decltype(auto) with_filters(Filter<subscription_ts>... filters){
	using follow_t = ScreenBuilderState1<SubscribeTo<subscription_ts...> >; 
	return follow_t{std::make_tuple(filters...)};
      };

      ScreenBuilderState1<SubscribeTo<>> without_filters(){
	return ScreenBuilderState1<SubscribeTo<>>{std::make_tuple()};
      };

    };

    } // screen
  }//widgets
  
  namespace widgets{

    namespace window {
      struct WindowSettings: public BaseSettings{
	Size size;
      };

      class WindowReturnContainer {
	using widgets_vector_t = std::vector<std::unique_ptr<BaseWidget>> ;
      public:
      WindowReturnContainer(WindowSettings settings , widgets_vector_t widgets)
	: settings_(settings),
	  widgets_(std::move(widgets)) {};
    private:
      widgets_vector_t widgets_;
      WindowSettings settings_ ;
    };
      
      
      namespace detail{
      
	template<class ...Arg_ts>
	struct WindowReturnContainerMaker;

      
      
	template<class ...Ts>
	struct WindowReturnContainerMaker<Size,Ts...>{
	  static WindowReturnContainer make(Size size, Ts...widgets ){
	    std::vector<BaseWidget*> vec1{
	      static_cast<BaseWidget*>(new Ts{std::move(widgets)})...
		};
	  
	    return WindowReturnContainer(
		WindowSettings{.size=size},
		std::vector<std::unique_ptr<BaseWidget>>(vec1.cbegin(),vec1.cend())
	    );
	  }
	};
      }//detail
      
      template <class ...Arg_ts>
      WindowReturnContainer make_window_return_container( Arg_ts... args){
       
	return detail::WindowReturnContainerMaker<Arg_ts...>::make( std::move(args)...);
      };
      
      template<class subscriptions_t, class listened_for_t>
      struct  Window:public WindowBase{
      private:
	template<class ...F>
	using to_function = Function<WindowReturnContainer, F...>;
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
      Window(filters_tuple_t filters,
	     function_t render_function,
	     listener_tuple_t listeners) :
	filters_(std::move(filters)),
	render_function_ (std::move(render_function)),
	listeners_(std::move(listeners)){};
      
      	std::vector<const std::string*> get_subscriptions()const{
	  return {};
	}
	
	void accept(RenderVisitor& visitor ){
	  visitor.render_widget(this->filters_, this->render_function_ );
	};

    private:
      filters_tuple_t filters_;
      function_t render_function_;
      listener_tuple_t listeners_;
    };
      
    
      struct WindowBuilder{
    private:
      template<class ...F>
      using to_fn_ptr = WindowReturnContainer(*)(F... );
     
      template<class ...F>
      using to_closure_maker = ClosureMaker<WindowReturnContainer, F...>;
     
      template<class ...F>
      using to_function = Function<WindowReturnContainer, F...>;
     
      template <class T>
      using to_event_handler = EventHandler<AppEvent,T>;
    public:
      // OK, normally a Builder returns itself. We do this a bit differently here as the builder also collects the types of its arguments.
      // The builder has 4 main states.
      //   state 0 (WindowBuilder) the sourrounding state
      //   state 1 when Filters are set
      //   state 2 when also the render function is set
      //   state 2.5 when the subscribed to EventTypes are set
      //   // state 3 when the event handler are set. we currently build directly.

      template <class subscriptions_t, class listened_for_t>
      struct WindowBuilderState2_5;

      template < class subscriptions_t,  class ... listened_for_ts  >
      struct WindowBuilderState2_5<subscriptions_t, ListenFor<listened_for_ts...>>{
      private:
	using listened_for_t = ListenFor<listened_for_ts...>;
       
	using filters_tuple_t = typename subscriptions_t
	  ::template map<Filter>
	  ::template apply<std::tuple>;
       
	using function_t = typename subscriptions_t
	  ::template apply<to_function> ;
      public:
	WindowBuilderState2_5(filters_tuple_t filters,
			      function_t function)
	  :filters(filters),
	   render_function(std::move(function)){};
       
	template<class ...Arg_ts>
	decltype(auto) build_with_event_handling_lambdas(Arg_ts...args){

	  auto tpl =  std::make_tuple(EventHandler<AppEvent, listened_for_ts>(args ) ... );

	  return Window<subscriptions_t, listened_for_t>(std::move(this->filters),

							 std::move(this->render_function),
							 std::move(tpl)
	  );
	};
	filters_tuple_t filters;
	function_t render_function;

      };

      // sub_t is for a SubscribeTo<...> class
      template <class sub_t>
      struct WindowBuilderState2{
      private:
	using filters_tuple_t = typename sub_t
	  ::template map<Filter>
	  ::template apply<std::tuple>;
	using function_t = typename sub_t
	  ::template apply<to_function> ;
      public:
	WindowBuilderState2(filters_tuple_t filters,
			    function_t function):
	  filters(filters),
	  render_function(std::move(function)){
	};

	decltype(auto) build_without_event_handlers(){
	  return this->for_events<>().build_with_event_handling_lambdas();
	}

	template<class ...listened_for_ts>
	decltype(auto) for_events(){
	  using state2_5_builder_t = WindowBuilderState2_5<sub_t, ListenFor<listened_for_ts...>>; 
	  return state2_5_builder_t(this->filters, std::move(this->render_function) );
	};
	
	filters_tuple_t filters;
	function_t render_function;
      };

      template <class sub_t>
      struct WindowBuilderState1{
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
	 
	  return WindowBuilderState2<sub_t>(
	      this->filters,
	      closure_maker_t::make(fn_ptr).as_fun()
	  );
	};

        filter_tuple_t filters;
      };

      template<class ... subscription_ts>
      decltype(auto) with_filters(Filter<subscription_ts>... filters){
	using follow_t = WindowBuilderState1<SubscribeTo<subscription_ts...> >; 
	return follow_t{std::make_tuple(filters...)};
      };

      /*
      decltype(auto) without_filters(){
	return this->with_filters();
      };
      */
      WindowBuilderState1<SubscribeTo<>> without_filters(){
	return WindowBuilderState1<SubscribeTo<>>{std::make_tuple()};
      };

    };

      
    }// window
  }// widgets
  
  // application
  namespace widgets{
    namespace application{

      struct ApplicationSettings: public BaseSettings{

      };
      
      class ApplicationReturnContainer {
	using widgets_vector_t = std::vector<std::unique_ptr<screen::ScreenBase>> ;
      public:
	ApplicationReturnContainer( ApplicationSettings ,widgets_vector_t widgets)
	  : widgets_(std::move(widgets)) {};
      private:
	widgets_vector_t widgets_;
      };

 
      namespace detail{
      
      template<class ...Arg_ts>
      struct ApplicationReturnContainerMaker;
      
      template<class ...window_ts>
      struct ApplicationReturnContainerMaker<ApplicationSettings, window_ts... >{
	static ApplicationReturnContainer make( window_ts...screens ){
	  
	  std::vector<screen::ScreenBase*> vec1{
	    static_cast<screen::ScreenBase*>(new window_ts(std::move(screens)))...
	      };
	  
	  return ApplicationReturnContainer(
	      ApplicationSettings{},std::vector<std::unique_ptr<screen::ScreenBase>>(vec1.cbegin(),vec1.cend())
	  );
	}
      };
    }//detail
    
      template <class ...Arg_ts>
      ApplicationReturnContainer make_application_return_container(  Arg_ts... args){
       
      return detail::ApplicationReturnContainerMaker<ApplicationSettings,Arg_ts...>::make( std::move(args)...);
    };
    }// application
    
    namespace application{
      
      
      template<class subscriptions_t, class listened_for_t>
      struct  Application:public ApplicationBase{
      private:
	template<class ...F>
	using to_function = Function<ApplicationReturnContainer, F...>;
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

        Application(filters_tuple_t filters,
		    function_t render_function,
		    listener_tuple_t listeners) :
	  filters_(std::move(filters)),
	  render_function_ (std::move(render_function)),
	  listeners_(std::move(listeners)){};
	std::vector<const std::string*> get_subscriptions()const{
	  return {};
	}
	void accept(RenderVisitor& visitor ){
	  visitor.render_widget(this->filters_, this->render_function_ );
	};

      private:
	filters_tuple_t filters_;
	function_t render_function_;
	listener_tuple_t listeners_;
      };

    
      struct ApplicationBuilder{
      private:
	template<class ...F>
	using to_fn_ptr = ApplicationReturnContainer(*)(F... );
     
	template<class ...F>
	using to_closure_maker = ClosureMaker<ApplicationReturnContainer, F...>;
     
	template<class ...F>
	using to_function = Function<ApplicationReturnContainer, F...>;
     
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
	struct ApplicationBuilderState2_5;

	template < class subscriptions_t,  class ... listened_for_ts  >
	struct ApplicationBuilderState2_5<subscriptions_t, ListenFor<listened_for_ts...>>{
	private:
	  using listened_for_t = ListenFor<listened_for_ts...>;
       
	  using filters_tuple_t = typename subscriptions_t
	    ::template map<Filter>
	    ::template apply<std::tuple>;
       
	  using function_t = typename subscriptions_t
	    ::template apply<to_function> ;
	public:
	  ApplicationBuilderState2_5(filters_tuple_t filters,
				function_t function)
	    :filters(filters),
	     render_function(std::move(function)){};
       
	  template<class ...Arg_ts>
	  decltype(auto) build_with_event_handling_lambdas(Arg_ts...args){

	    auto tpl =  std::make_tuple(EventHandler<AppEvent, listened_for_ts>(args ) ... );

	    return Application<subscriptions_t, listened_for_t>(std::move(this->filters),

							   std::move(this->render_function),
							   std::move(tpl)
	    );
	  };
	  filters_tuple_t filters;
	  function_t render_function;

	};

	// sub_t is for a SubscribeTo<...> class
	template <class sub_t>
	struct ApplicationBuilderState2{
	private:
	  using filters_tuple_t = typename sub_t
	    ::template map<Filter>
	    ::template apply<std::tuple>;
	  using function_t = typename sub_t
	    ::template apply<to_function> ;
	public:
	  ApplicationBuilderState2(filters_tuple_t filters,
			      function_t function):
	    filters(filters),
	    render_function(std::move(function)){
	  };

	  decltype(auto) build_without_event_handlers(){
	    return this->for_events<>().build_with_event_handling_lambdas();
	  }
	  template<class ...listened_for_ts>
	  decltype(auto) for_events(){
	    using state2_5_builder_t = ApplicationBuilderState2_5<sub_t, ListenFor<listened_for_ts...>>; 
	    return state2_5_builder_t(this->filters, std::move(this->render_function) );
	  };

	  filters_tuple_t filters;
	  function_t render_function;
	};

	template <class sub_t>
	struct ApplicationBuilderState1{
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
	 
	    return ApplicationBuilderState2<sub_t>(
		this->filters,
		closure_maker_t::make(fn_ptr).as_fun()
	    );
	  };

	  filter_tuple_t filters;
	};

	template<class ... subscription_ts>
	decltype(auto) with_filters(Filter<subscription_ts>... filters){
	  using follow_t = ApplicationBuilderState1<SubscribeTo<subscription_ts...> >; 
	  return follow_t{std::make_tuple(filters...)};
	};

	ApplicationBuilderState1<SubscribeTo<>> without_filters(){
	  return ApplicationBuilderState1<SubscribeTo<>>{std::make_tuple()};
	};

      };

    } //application
  }// widget

} // fluxpp




#endif //FLUXPP_WIDGET_HPP
