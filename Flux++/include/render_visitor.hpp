#ifndef FLUXPP_RENDER_VISITOR_HPP
#define FLUXPP_RENDER_VISITOR_HPP
#include <mem_comparable_closure.hpp>
#include <boost/uuid/uuid.hpp>

#include "backend/base_backend.hpp"
#include "state.hpp"
#include "widget_fwd.hpp"

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
    class RenderVisitor{
     
    public:
      void visit_base(uuid_t){};
      template<class ... Arg_ts>
      void render_widget(const std::tuple<Filter<Arg_ts>...>&,
			const Function<WidgetReturnContainer, Arg_ts...>& render_fn){
	
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
      
    private:
      backend::AsynchronousBackendInterfaceBase * backend_aifc;
      //      state::SynchronousStateInterface* state_ifc;
      //      std::set<uuid_t,WidgetNode> ;

      
    };
    

  }

}
#endif //FLUXPP_RENDER_VISITOR_HPP
