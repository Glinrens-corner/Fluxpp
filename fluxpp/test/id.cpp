#include <doctest/doctest.h>
#include "id.hpp"

struct TestTag{};

TEST_CASE("id" ){
  using namespace fluxpp::id;
  SequentialIdGenerator<TestTag> sequential_id_generator{}; 


};
