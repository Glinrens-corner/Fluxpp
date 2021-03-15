#ifndef FLUXPP_RENDER_VISITOR_HPP
#define FLUXPP_RENDER_VISITOR_HPP
#include <mem_comparable_closure.hpp>
#include <boost/uuid/uuid.hpp>

#include "backend/base_backend.hpp"
#include "state.hpp"
#include "widget_fwd.hpp"
#include "render_tree.hpp"

namespace fluxpp{

  namespace visitors{
    using mem_comparable_closure::Function;
    using  widgets::WidgetReturnContainer;
    using  widgets::Filter;
    using namespace widgets::builtin;
    using  widgets::screen::ScreenReturnContainer;
    using  widgets::application::ApplicationReturnContainer;
    using  widgets::window::WindowReturnContainer;
    using uuid_t = boost::uuids::uuid;
    struct RenderIFCHolder;
    
    class RenderVisitor{

    private:
      template<class Arg_t >
      Arg_t get_state_slice_state(Filter<Arg_t> filter ){
	return Arg_t{};
      };

      void process_container(WidgetReturnContainer, uuid_t );
      
      void process_container(widgets::application::ApplicationReturnContainer);
      uuid_t process_container(
	  widgets::window::WindowReturnContainer,
	  std::unique_ptr<widgets::window::WindowBase> ,
	  uuid_t 
      );
      
      uuid_t process_container(
	  widgets::screen::ScreenReturnContainer,
	  std::unique_ptr<widgets::screen::ScreenBase> screen,
	  uuid_t parent_uuid);
      
    public:
      RenderVisitor(RenderIFCHolder* ifcs, uuid_t uuid)
	:render_ifcs(ifcs)
	,visiting(uuid)
      {};
      
      
      template<class ... Arg_ts>
     uuid_t render_widget(
	 std::unique_ptr<widgets::BaseWidget> base,
	 const std::tuple<Filter<Arg_ts>...>&filter_tuple,
	  
	  const Function<WidgetReturnContainer, Arg_ts...>& render_fn,
	  uuid_t parent_uuid){
	std::apply([this,render_fn , parent_uuid](Filter<Arg_ts>... filters){
	    return this->process_container(render_fn(this->get_state_slice_state(filters)...), parent_uuid);}, filter_tuple);
	return uuid_t{};
      };

      
      template<class ... Arg_ts>
      void render_widget(
	  const std::tuple<Filter<Arg_ts>...>& filter_tuple,
	  const Function<ApplicationReturnContainer, Arg_ts...>& render_fn){
	std::apply([this,render_fn ](Filter<Arg_ts>... filters){
	    return this->process_container(render_fn(this->get_state_slice_state(filters)...));}, filter_tuple);
      };

      template<class ... Arg_ts>
      uuid_t render_widget(
	  std::unique_ptr<widgets::window::WindowBase> window,
	  const std::tuple<Filter<Arg_ts>...>& filter_tuple,
	  const Function<WindowReturnContainer, Arg_ts...>& render_fn,
	  uuid_t parent_uuid){
	
	return std::apply([this,render_fn, &window, parent_uuid ](Filter<Arg_ts>... filters)->uuid_t{
	    return this->process_container(
		render_fn(this->get_state_slice_state(filters)...),
		std::move(window),
		parent_uuid
	    );}, filter_tuple);
	return uuid_t{};
      };

      template<class ... Arg_ts>
      uuid_t render_widget(
	  std::unique_ptr<widgets::screen::ScreenBase> screen,
	  const std::tuple<Filter<Arg_ts>...>& filter_tuple,
	  const Function<ScreenReturnContainer, Arg_ts...>& render_fn,
	  uuid_t parent_uuid){
	
	return std::apply([this,render_fn, &screen, parent_uuid ](Filter<Arg_ts>... filters)->uuid_t{
	    return this->process_container(
		render_fn(this->get_state_slice_state(filters)...),
		std::move(screen),
		parent_uuid
	    );}, filter_tuple);

	return uuid_t{};
      };

      
      
      uuid_t render_widget(
	  const ColorWidget& widget,
	  std::unique_ptr<widgets::BaseWidget> base,
	  uuid_t parent_uuid){
	return uuid_t{};
      };
      
      uuid_t render_widget(
	  const TextWidget& widget,
	  std::unique_ptr<widgets::BaseWidget> base,
	  uuid_t parent_uuid){
	return uuid_t{};
      };
      
      void visit( widgets::application::ApplicationBase *);

      uuid_t visit( std::unique_ptr<widgets::screen::ScreenBase> screen, uuid_t parent_uuid);
      uuid_t visit( std::unique_ptr<widgets::window::WindowBase> screen, uuid_t parent_uuid);

      uuid_t visit( std::unique_ptr<widgets::BaseWidget> screen, uuid_t parent_uuid);

      
    private:
      uuid_t visiting ;
      RenderIFCHolder * render_ifcs;
    };
    
  }

}
#endif //FLUXPP_RENDER_VISITOR_HPP
