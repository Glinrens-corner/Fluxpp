#include "doctest.h"

#include "Lambda.hpp"
#include <type_traits>



TEST_CASE("Lambda"){
  using namespace fluxpp;



  SUBCASE("creation"){
    using namespace transparent_closure;
    using closure_t = Closure<
      FunctionSignature<int,int,int>>;
    SUBCASE("closure_from_fp"){
      int(*fp)(int,int)  = [](int a,int b)->int{return static_cast<char>(a);};
      auto closure = closure_from_fp(fp);
      CHECK(std::is_same<decltype(closure), closure_t>::value);
    
    };
    
    auto closure = ClosureMaker<int,int,int>::make([](int a, int b){return a; } );
    CHECK(std::is_same<decltype(closure), closure_t>::value);
    CHECK(closure(1,2 ) == 1);
    auto new_closure = closure.bind(2);
    CHECK(std::is_trivially_copyable<decltype(closure)>::value );
    CHECK(std::is_trivially_copyable<decltype(new_closure)>::value );
    CHECK(new_closure(3) == 2);

    //    auto closure2 = ClosureMaker<int&,int,int>::make([](int a, int b )->int{return b;});
    //    auto closure3 = ClosureMaker<int,int,int>::make([](int& a, int b )->int{return b;});
  };
  SUBCASE("Metaprogramming Errors"){ 
    using namespace transparent_closure;
    // we would need to check that this gives an compiler error
    //    using h = test::check_transparency<int&>;
    // this however does not.
    // using g = test::check_transparency<int>;
  };
					    
  SUBCASE("check equality"){
    using namespace transparent_closure;
    int (*fn)(int,int) = [](int a, int b ) -> int { return a;};
    auto closure1 =ClosureMaker<int,int,int>::make(fn).bind(2).as_fun();
    auto closure2=ClosureMaker<int,int,int>::make(fn).bind(2).as_fun();;
    auto closure3=ClosureMaker<int,int,int>::make(fn).bind(3).as_fun();;
    auto mem_info1 = closure1.get_mem_compare_info();
    auto mem_info2 = closure2.get_mem_compare_info();
    auto mem_info3 = closure3.get_mem_compare_info();
    REQUIRE(mem_info1.size==mem_info2.size );
    CHECK(std::memcmp(mem_info1.obj, mem_info2.obj, mem_info2.size) == 0);
    REQUIRE(mem_info2.size==mem_info3.size );
    CHECK(std::memcmp(mem_info2.obj, mem_info3.obj, mem_info2.size) != 0);
  };

}
