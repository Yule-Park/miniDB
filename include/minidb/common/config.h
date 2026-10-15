#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

namespace minidb {
constexpr std::size_t PAGE_SIZE = 4096;
using page_id_t = std::uint32_t;
constexpr page_id_t INVALID_PAGE_ID = std::numeric_limits<page_id_t>::max();
}