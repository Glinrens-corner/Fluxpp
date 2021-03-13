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

      void process_container(WidgetReturnContainer&& );
      
    public:
      RenderVisitor(RenderIFCHolder* ifcs, uuid_t uuid)
	:render_ifcs(ifcs)
	,visiting(uuid)
      {};

      
      template<class ... Arg_ts>
      void render_widget(
	  const std::tuple<Filter<Arg_ts>...>&filter_tuple,
	  const Function<WidgetReturnContainer, Arg_ts...>& render_fn){
	std::apply([this,render_fn ](Filter<Arg_ts>... filters){
	    return this->process_container(render_fn(this->get_state_slice_state(filters)...));
	  },  filter_tuple);
      };

      
      template<class ... Arg_ts>
      void render_widget(const std::tuple<Filter<Arg_ts>...>&,
			const Function<ApplicationReturnContainer, Arg_ts...>& render_fn){
	
      };

      template<class ... Arg_ts>
      void render_widget(const std::tuple<Filter<Arg_ts>...>&,
			const Function<WindowReturnContainer, Arg_ts...>& render_fn){
	
      };

      template<class ... Arg_ts>
      void render_widget(const std::tuple<Filter<Arg_ts>...>&,
			const Function<ScreenReturnContainer, Arg_ts...>& render_fn){
	
      };

      
      
      void render_widget(const ColorWidget& widget){
	
      };
      void render_widget(const TextWidget& widget){
	
      };
      
      void visit();
    private:
      uuid_t visiting ;
      RenderIFCHolder * render_ifcs;
    };
    

  }

}
#endif //FLUXPP_RENDER_VISITOR_HPP
