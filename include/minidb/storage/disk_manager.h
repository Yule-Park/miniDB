#pragma once
#include "config.h"
#include "page.h"
#include <string>
#include <fstream>
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
