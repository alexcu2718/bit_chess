#include "bit_chess_cpp/bit_chess_cpp.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace bit_chess_cpp;

TEST_CASE( "add_one", "[adder]" ){
  REQUIRE(add_one(0) == 1);
  REQUIRE(add_one(123) == 124);
  REQUIRE(add_one(-1) == 0);
}
