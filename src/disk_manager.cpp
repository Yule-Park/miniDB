#include "minidb/storage/disk_manager.h"


namespace minidb {

DiskManager::DiskManager(const std::string& file_name)
    : file_name_(file_name) {
    if (!std::filesystem::exists(file_name_)) {
        std::ofstream file(file_name_, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create file: " + file_name_);
        }
    }
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_name_);
    }
    if (std::filesystem::file_size(file_name_) % PAGE_SIZE != 0) {
        throw std::runtime_error("File size is not a multiple of page size");
    }
}
page_id_t DiskManager::allocate_page() {
    auto pages = std::filesystem::file_size(file_name_) / PAGE_SIZE;
    if (pages >= std::numeric_limits<page_id_t>::max()) {
        throw std::runtime_error("Maximum number of pages reached");
    }
    page_id_t new_page_id = static_cast<page_id_t>(pages);
    std::array<char, PAGE_SIZE> empty_page{};
    file_.seekp(0, std::ios::end);
    if (!file_) {
        throw std::runtime_error("Failed to seek to end of file for page allocation");
    }
    file_.write(empty_page.data(), PAGE_SIZE);
    if (!file_) {
        throw std::runtime_error("Failed to allocate new page");
    }
    file_.flush();
    if (!file_) {
        throw std::runtime_error("Failed to flush after allocating new page");
    }
    return new_page_id;
}
void DiskManager::read_page(page_id_t page_id, Page& page) {
    auto pages = std::filesystem::file_size(file_name_) / PAGE_SIZE;
    if (static_cast<std::uintmax_t>(page_id) >= pages) {
        throw std::out_of_range("Page ID is out of valid range");
    }
    std::streamoff offset = static_cast<std::streamoff>(page_id) * PAGE_SIZE;
    file_.seekg(offset, std::ios::beg);
    if (!file_) {
        throw std::runtime_error("Failed to seek to page " + std::to_string(page_id));
    }
    file_.read(reinterpret_cast<char*>(page.data()), PAGE_SIZE);
    if (!file_) {
        throw std::runtime_error("Failed to read page " + std::to_string(page_id));
    }
}
void DiskManager::write_page(page_id_t page_id, const Page& page) {
    auto pages = std::filesystem::file_size(file_name_) / PAGE_SIZE;
    if (static_cast<std::uintmax_t>(page_id) >= pages) {
        throw std::out_of_range("Page ID is out of valid range");
    }
    std::streamoff offset = static_cast<std::streamoff>(page_id) * PAGE_SIZE;
    file_.seekp(offset, std::ios::beg);
    if (!file_) {
        throw std::runtime_error("Failed to seek to page " + std::to_string(page_id));
    }
    file_.write(reinterpret_cast<const char*>(page.data()), PAGE_SIZE);
    if (!file_) {
        throw std::runtime_error("Failed to write to page");
    }
    file_.flush();
    if (!file_) {
        throw std::runtime_error("Failed to flush page");
    }
}

}