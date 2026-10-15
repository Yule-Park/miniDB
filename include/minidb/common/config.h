#pragma once

#include <cstddef>
#include <cstdint>

namespace minidb {
constexpr std::size_t PAGE_SIZE = 4096;
constexpr std::size_t INVALID_PAGE_ID = static_cast<std::size_t>(-1);
using page_id_t = std::uint32_t;
}