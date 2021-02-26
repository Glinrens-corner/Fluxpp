#include "transparent_closure.hpp" 

//namespace  {
  bool fluxpp::transparent_closure::detail::is_identical_object(MemCompareInfo& info1, MemCompareInfo& info2){
    if (info1.size != info2.size)return false;
    
    assert(info1.size == info2.size);
    if(std::memcmp(info1.obj,info2.obj, info2.size) !=0 ) return false;
    
    return true;
  };
//}
