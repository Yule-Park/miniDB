#pragma once

#include <cstddef>
#include <cstdint>

namespace minidb {
constexpr std::size_t PAGE_SIZE = 4096;
using page_id_t = std::uint32_t;
}