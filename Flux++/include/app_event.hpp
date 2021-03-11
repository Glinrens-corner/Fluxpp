#ifndef FLUXPP_APP_EVENT_HPP
#define FLUXPP_APP_EVENT_HPP


namespace fluxpp{
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
  } // widgets
} // fluxpp











#endif FLUXPP_APP_EVENT_HPP
