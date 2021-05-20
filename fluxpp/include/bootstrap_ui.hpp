#ifndef FLUXPP_BOOTSTRAP_UI_HPP
#define FLUXPP_BOOTSTRAP_UI_HPP
#include <string>


namespace fluxpp{
  namespace bootstrap_ui {

    // abstract baseclass for BootstrapUI
    // each message takes a classifier their semantics are currently undefined.
    class BootstrapUi {
    public:
      // errors are unrecoverable failures
      virtual void error(char classifier, const std::string& msg ) = 0;
      //  warnings are recoverable errors (often incorrect api use) 
      virtual void warning(char classifier, const std::string& msg) = 0;
      // info is  for general information
      virtual void info(char classifier, const std::string& msg) = 0;
      // debug is for debug information
      // note that debugging must be enabled to generate these messages
      virtual void debug(char classifier, const std::string& msg) = 0;
    };
  }//bootstrap_ui
}//fluxpp




#endif //FLUXPP_BOOTSTRAP_UI_HPP
