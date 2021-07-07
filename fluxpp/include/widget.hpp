#ifndef FLUXPP_WIDGET_HPP
#define FLUXPP_WIDGET_HPP
#include "meta.hpp"
#include "event_system.hpp"
#include <functional>

namespace fluxpp{
  namespace widget{
#define FLUXPP_WIDGET_SUPERCLASS_VALUES        \
    FLUXPP_WIDGET_SUPERCLASS_X_FIRST(application )   \
    FLUXPP_WIDGET_SUPERCLASS_X(client )        \
    FLUXPP_WIDGET_SUPERCLASS_X(window )        \
    FLUXPP_WIDGET_SUPERCLASS_X_LAST(visual_2d )        
    

#define FLUXPP_WIDGET_SUPERCLASS_X(name) name,
#define FLUXPP_WIDGET_SUPERCLASS_X_FIRST(name) name,
#define FLUXPP_WIDGET_SUPERCLASS_X_LAST(name) name
    
    enum class WidgetSuperclass{
      FLUXPP_WIDGET_SUPERCLASS_VALUES
    };
#undef FLUXPP_WIDGET_SUPERCLASS_X
#undef FLUXPP_WIDGET_SUPERCLASS_X_FIRST
#undef FLUXPP_WIDGET_SUPERCLASS_X_LAST

    namespace detail{      
      template<class return_t , class... argument_ts>
      struct FluxppFunction{

        // TODO include transparent_closure::Function
        std::function<return_t(argument_ts...)> function;
      };

    };

    
    template<class return_t >
    class AbstractSubscription {};


    template<class return_t, class input_t>
    class Subscription: public AbstractSubscription<return_t>{


      fluxpp::event_system::Path path;
      detail::FluxppFunction<return_t,input_t> function;
    };

    namespace detail{
    

      template<class return_t>
      struct SubscriptionContainer{

        // the return_t == input_t is probably the most common case,
        // so lets allow to handle that without heap allocation.
        std::variant<
          Subscription<return_t,return_t>,
          std::unique_ptr<AbstractSubscription< return_t>>

          > subscription;
        
      };

      template<class return_t>
      struct to_fluxpp_function_type_variadic{

        template<class ... Ts>
        using fn = FluxppFunction<return_t(Ts...) >;

      };
      template<class return_t>
      struct to_fluxpp_function_type{
        template<class list_t>
        using fn  = typename list_t::template apply<
          to_fluxpp_function_type_variadic<return_t>::template fn>;
      };



    };

    
    template<WidgetSuperclass widget_superclass_>
    class FullyErasedWidget{};

    template<WidgetSuperclass widget_superclass_, class return_t>
    class PartiallyErasedWidget:public FullyErasedWidget<widget_superclass_>{
      
    };
    
    
    // subscriptions : meta::type_list of return_types of subscriptions or abstract subscriptions
    template<WidgetSuperclass widget_superclass_, class return_t, class subscriptions>
    class Widget: public PartiallyErasedWidget<widget_superclass_, return_t>{
    private:
      using subscription_container_tuple_t = fluxpp::meta::apply_to_t<
        subscriptions,
      fluxpp::meta::map<detail::SubscriptionContainer >::template fn
        >;

    public:


    private:
      
    };


    
    
  }// widget
}//fluxpp


#endif// FLUXPP_WIDGET_HPP
