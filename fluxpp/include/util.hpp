#ifndef FLUXPP_UTIL_HPP
#define FLUXPP_UTIL_HPP

#include <type_traits>
#include <tuple>
#include <optional>
// the nuts and bolts of the framework,
// small  
namespace fluxpp {
  namespace util {
    enum class InterfaceType {
      Asynchronous, 
      Locked,
      Committing
    };


    struct Color{
    public:
      static constexpr uint8_t color_max_value = 255;
    private:
      constexpr Color(uint8_t red,
            uint8_t green,
            uint8_t blue,
            uint8_t alpha):
        red_(red),
        green_(green),
        blue_(blue),
        alpha_(alpha){};
    public:
      static constexpr Color from_rgba(uint8_t red,
                                       uint8_t green,
                                       uint8_t blue,
                                       uint8_t alpha){
        return Color(red, green, blue, alpha);
      };
      static constexpr Color from_rgb(uint8_t red,
                                      uint8_t green,
                                      uint8_t blue){
        return Color(red, green, blue, color_max_value );
      };

    public:
      uint8_t red()const{return this->red_;};
      uint8_t green()const{return this->green_;};
      uint8_t blue()const{return this->blue_;};
      uint8_t alpha()const{return this->alpha_;};

    public:
      bool operator== ( const Color& other)const{
        return (this->red_ == other.red_ )
          and  (this->green_ == other.green_)
          and  (this->blue_ == other.blue_)
          and  (this->alpha_ == other.alpha_);
      };
    private:
      uint8_t red_;
      uint8_t green_;
      uint8_t blue_;
      uint8_t alpha_;
    };
    
    enum class ColorNamespace {
      basic
    };
    
    template <ColorNamespace >
    struct NamedColor;

    template<>
    struct NamedColor<ColorNamespace::basic>{
      NamedColor() = delete;
      static constexpr Color red = Color::from_rgb(Color::color_max_value, 0,0) ;
      static constexpr Color green = Color::from_rgb(0, Color::color_max_value,0) ;
      static constexpr Color blue = Color::from_rgb(0, 0,Color::color_max_value) ;
      static constexpr Color white = Color::from_rgb(Color::color_max_value, Color::color_max_value,Color::color_max_value);
      static constexpr Color black = Color::from_rgb(0, 0,0);
    };
    
    struct Length {
    public:
      Length(){};
      // explicitly not explicit...
      Length(uint64_t length): length_(length){};

      bool is_defined( )const{return this->length_.has_value();};
      uint8_t value()const{return this->length_.value();};

      template <class function_t >
      void invoke_if_defined(function_t function )const{
        if (this->length_.has_value()){
          function(this->length_.value() );
        };
      };

      template <class function_t >
      void invoke_if_undefined(function_t function )const{
        if ( not this->length_.has_value()){
          function( );
        };
      };
      
      template <class if_function_t , class else_function_t>
      typename std::invoke_result<else_function_t>::type if_else(
          if_function_t if_function ,
          else_function_t else_function )const{
        static_assert(
            std::is_same<
            typename std::invoke_result<if_function_t, uint64_t >::type,
            typename std::invoke_result<else_function_t >::type>::value,
        "if function and else function must return the same type" );
        if (this->length_.has_value() ){
          return if_function( this->length_.value() );
        } else {
          return else_function();
        };
      };

      
    private:
      std::optional<uint64_t> length_{};
    };
    
    struct Extend2D {
    public:
      Extend2D( Length width, Length height):
        width_(width),
        height_(height){};
      Extend2D() {};

      Length width()const{ return this->width_;};
      Length height()const{ return this->height_;};
      
    private:
      Length width_{};
      Length height_{};
    };
  }// util
}//fluxpp

// some definitions f
#include <transparent_closure/function.hpp>

#define MYVAR 0 
#endif // FLUXPP_UTIL_HPP