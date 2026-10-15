#pragma once
#include "minidb/common/config.h"
#include "minidb/storage/page.h"
#include <string>
#include <fstream>
#include <limits>
#include <array>
#include <filesystem>
#include <stdexcept>
namespace minidb {
    class DiskManager {
        public:
            explicit DiskManager(const std::string& file_name);
            void read_page(page_id_t page_id, Page& page);
            void write_page(page_id_t page_id, const Page& page);
            page_id_t allocate_page();
        private:
            std::string file_name_;
            std::fstream file_;
};
}
