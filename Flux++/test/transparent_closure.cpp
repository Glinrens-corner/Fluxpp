#include <doctest/doctest.h>

#include "transparent_closure.hpp"
#include <type_traits>

TEST_CASE("StackAllocator"){
  using  fluxpp::transparent_closure::detail::StackAllocator;
  SUBCASE("basic test"){
    StackAllocator stack{};
    

    REQUIRE(stack.get_size()==0 );
    new (stack.get_new<int>()) int{5};
    CHECK(stack.get_size() >= sizeof(int));
    CHECK( (*stack.get_last<int>())== 5);
    stack.pop_last<int>();
    CHECK(stack.get_size() == 0);
  };
  SUBCASE("multiple"){
    StackAllocator stack{};

    //    

    new (stack.get_new<long>()) long{5};
    new (stack.get_new<int>()) int{4};
    new (stack.get_new<short>()) short{3};
    CHECK( (*stack.get_last<short>())== 3);
    stack.pop_last<short>();
    CHECK( (*stack.get_last<int>())== 4);
    stack.pop_last<int>();
    CHECK( (*stack.get_last<long>())== 5);
    stack.pop_last<long>();
  };
  SUBCASE("basic test"){
    StackAllocator stack{};
    constexpr std::size_t stack_init_max_size = StackAllocator::get_init_max_size();
    new (stack.get_new<int>()) int{4};
    stack.get_new<char[stack_init_max_size]>();
    CHECK( stack.get_size()>=(stack_init_max_size+sizeof(int)));
    new (stack.get_new<int>()) int{16};
    CHECK( (*stack.get_last<int>())== 16);
    stack.pop_last<int>();
    stack.pop_last<char[stack_init_max_size]>();
    CHECK( (*stack.get_last<int>())== 4);
    stack.pop_last<int>();
    CHECK(stack.get_size() == 0 );
  };
}

TEST_CASE("Lambda"){
  using namespace fluxpp;



  SUBCASE("creation"){
    using namespace transparent_closure;
    using closure_t = Closure<
      FunctionSignature<int,int,int> >;

    SUBCASE("closure_from_fp"){
      int(*fp)(int,int)  = [](int a,int b)->int{return static_cast<char>(a);};
      auto closure = closure_from_fp(fp);
      CHECK(std::is_same<decltype(closure), closure_t>::value);
    };
    using closure_t = Closure<
      FunctionSignature<int,int,int> >;
    
    auto closure = ClosureMaker<int,int,int>::make([](int a, int b){return a; } );
    CHECK(std::is_same<decltype(closure), closure_t>::value);
    CHECK(closure(1,2 ) == 1);
    auto new_closure = closure.bind(2);
    CHECK(std::is_trivially_copyable<decltype(closure)>::value );
    CHECK(std::is_trivially_copyable<decltype(new_closure)>::value );
    CHECK(new_closure(3) == 2);

  };
  SUBCASE("Metaprogramming Errors"){ 
    using namespace transparent_closure;
    // we would need to check that this gives an compiler error
    //    using h = test::check_transparency<int&>;
    // this however does not.
    using g = test::check_transparency<int>;
    // Compiler Errors
    //    auto closure2 = ClosureMaker<int&,int,int>::make([](int a, int b )->int{return b;});
    //     int& is not transparent
    //    auto closure3 = ClosureMaker<int,int,int>::make([](int& a, int b )->int{return b;});
    //     int& is not transparent
  };
					    
  SUBCASE("check equality"){
    using namespace transparent_closure;
    int (*fn)(int,int) = [](int a, int b ) -> int { return a;};
    auto closure1 =ClosureMaker<int,int,int>::make(fn).bind(2).as_fun();
    auto closure2=ClosureMaker<int,int,int>::make(fn).bind(2).as_fun();;
    auto closure3=ClosureMaker<int,int,int>::make(fn).bind(3).as_fun();;
    //    auto mem_info1 = closure1.get_mem_compare_info();
    //    auto mem_info2 = closure2.get_mem_compare_info();
    //    auto mem_info3 = closure3.get_mem_compare_info();

    CHECK_FALSE(is_updated( closure1,closure2) );
    CHECK(is_identical(closure1,closure2));
    //    REQUIRE(mem_info1.size==mem_info2.size );
    //    CHECK(std::memcmp(mem_info1.obj, mem_info2.obj, mem_info2.size) == 0);
    
    CHECK_FALSE(is_identical(closure2,closure3));
    CHECK(is_updated( closure2,closure3) );
    //    REQUIRE(mem_info2.size==mem_info3.size );
    //    CHECK(std::memcmp(mem_info2.obj, mem_info3.obj, mem_info2.size) != 0);
  };

}
