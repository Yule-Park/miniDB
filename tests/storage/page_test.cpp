#include <catch2/catch_test_macros.hpp>
#include "minidb/common/config.h"
#include "minidb/storage/page.h"

TEST_CASE("Page class functionality") {
    minidb::Page page;

    // Check that the data pointer is not null
    REQUIRE(page.data() != nullptr);

    // Check that the data is initialized to zero
    for (std::size_t i = 0; i < minidb::PAGE_SIZE; ++i) {
        REQUIRE(page.data()[i] == std::byte{0});
    }

    // Check that the data can be modified and read back correctly
    std::byte test_byte = std::byte{0xAB};
    page.data()[0] = test_byte;
    REQUIRE(page.data()[0] == test_byte);

    // Check that the last byte can be modified and read back correctly
    page.data()[minidb::PAGE_SIZE - 1] = std::byte{0x7F};
    REQUIRE(page.data()[minidb::PAGE_SIZE - 1] == std::byte{0x7F});

    // Check that the const version of data() works correctly
    const minidb::Page& const_page = page;
    REQUIRE(const_page.data()[0] == test_byte);
}
