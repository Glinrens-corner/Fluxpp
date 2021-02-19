#include "doctest.h"

#include "Lambda.hpp"
#include <type_traits>



TEST_CASE("Lambda"){
  using namespace fluxpp;



  SUBCASE("creation"){
    using namespace lambda;
    using function_signature = FunctionSignature<int,long,long>;
    using closure_holder_t = ClosureHolder<
      FunctionSignature<int,int,int>>;
    auto closure = closure_holder_t([](int a, int b){return a; } );
    CHECK(closure(1,2 ) == 1);
    auto new_closure = closure.bind(2);
    CHECK(std::is_trivially_copyable<decltype(closure)>::value );
    CHECK(std::is_trivially_copyable<decltype(new_closure)>::value );
    CHECK(new_closure(3) == 2);
    int (*fun)(int,int) = [](int a, int b)->int{return a;};
    //   auto fn = Fun<int,int,int>([](int a, int b)->int{return a;});
    CHECK(std::is_same<typename function_signature::return_type, int>::value);

    



  };


}
