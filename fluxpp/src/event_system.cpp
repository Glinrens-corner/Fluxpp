#include "event_system.hpp"


namespace fluxpp{


  namespace event_system{
    GlobalStringInterner::id_t GlobalStringInterner::intern_string(std::string){
      return 0;
    };

  }// event_system
}// fluxpp
