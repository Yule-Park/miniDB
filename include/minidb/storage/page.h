#pragma once
#include <array>
#include <cstddef>
#include "minidb/common/config.h"

namespace minidb {
    class Page {
        private: 
            std::array<std::byte, PAGE_SIZE> data_{};
        public:
            Page() = default;
            std::byte* data() {
                return data_.data(); 
            }
            const std::byte* data() const { 
                return data_.data(); 
            }   
    };
}