#include <catch2/catch_test_macros.hpp>
#include "minidb/common/config.h"

TEST_CASE("Catch2 is working") {
    REQUIRE(2 + 2 == 4);
}
TEST_CASE("Page size is correct") {
    REQUIRE(minidb::PAGE_SIZE == 4096);
}