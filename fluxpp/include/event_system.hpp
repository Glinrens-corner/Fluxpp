#ifndef FLUXPP_EVENT_SYSTEM_HPP
#define FLUXPP_EVENT_SYSTEM_HPP
#include <map>
#include <string>
#include "id.hpp"

namespace fluxpp {
  namespace event_system{
    
    class GlobalStringInterner{
      using id_t = fluxpp::id::id_t;
    private:
      GlobalStringInterner(){};
    public:
      GlobalStringInterner(const GlobalStringInterner &) = delete;
      void operator=(const GlobalStringInterner&) = delete; 
    public:
      static GlobalStringInterner& get_instance(){
        static GlobalStringInterner instance;
        return instance;
      };

    public:
      id_t intern_string(std::string name);
    private:
      id_t current_id_{0};
      std::map<id_t, std::string> string_by_id{ };
      bool is_sealed = false;
    };


  } // event_system
} // fluxpp






#endif // FLUXPP_EVENT_SYSTEM_HPP
