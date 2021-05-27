#ifndef FLUXPP_ERROR_HPP
#define FLUXPP_ERROR_HPP

#include <exception>

namespace fluxpp{
  namespace error{
    class sealed_access_exception: public std::exception{
      const char* what() const noexcept{ return "Tried to access sealed Singleton.";};
    };

  }//error
}//fluxpp






#endif // FLUXPP_ERROR_HPP
