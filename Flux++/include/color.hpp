#ifndef FLUXPP_COLOR_HPP
#define FLUXPP_COLOR_HPP

namespace fluxpp{
  namespace widgets{
    namespace builtin{
      enum class ColorEnum{
	black,
	white,
	blue,
	red,
	green
      };
    
      struct Color{
	static constexpr Color from_color_enum( ColorEnum color_enum){
	  switch(color_enum ){
	  case ColorEnum::black :
	    return Color{
	      .red=0 ,
		.green=0,
		.blue=0,
		.alpha=0xff};
	    break;
	  case ColorEnum::white :
	    return Color{
	      .red=0xff ,
		.green=0xff,
		.blue=0xff,
		.alpha=0xff};
	    break;
	  case ColorEnum::blue :
	    return Color{
	      .red=0 ,
		.green=0,
		.blue=0xff,
		.alpha=0xff};
	    break;
	  case ColorEnum::red :
	    return Color{
	      .red=0xff ,
		.green=0,
		.blue=0,
		.alpha=0xff};
	    break;
	  case ColorEnum::green :
	    return Color{
	      .red=0 ,
		.green=0xff,
		.blue=0,
		.alpha=0xff};
	    break;
	  };
	};
	uint8_t red=0;
	uint8_t green=0;
	uint8_t blue=0;
	uint8_t alpha=0;
      };
    }// builtin
 
  } // widgets
}// fluxpp
















#endif //FLUXPP_COLOR_HPP
