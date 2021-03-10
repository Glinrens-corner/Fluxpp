#ifndef XCBBACKEND_HPP
#define XCBBACKEND_HPP
#include "backend/base_backend.hpp"

namespace fluxpp{
  namespace backend{
    
    class XCBBackend: public BaseBackend{
    public:
      static XCBBackend create(){return XCBBackend{}; };

    };
  }
};



#endif //XCBBACKEND_HPP
