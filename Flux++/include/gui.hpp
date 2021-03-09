#ifndef GUI_HPP
#define GUI_HPP
#include "state.hpp"

namespace fluxpp{
  class Gui{
  public:
    template<class T>
    void add_state(state::State<T> state){}; 
  };
};















#endif // GUI_HPP
