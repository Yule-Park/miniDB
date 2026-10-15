#include <catch2/catch_test_macros.hpp>
#include <filesystem>

#include "minidb/common/config.h"
#include "minidb/storage/disk_manager.h"
#include "minidb/storage/page.h"

TEST_CASE("DiskManager page allocation works") {
    const std::string file_name = "file";
    std::filesystem::remove(file_name);
    {
        minidb::DiskManager disk_manager(file_name);
        REQUIRE(std::filesystem::file_size(file_name) == 0);
        minidb::page_id_t first_page_id = disk_manager.allocate_page();
        REQUIRE(first_page_id == 0);
        REQUIRE(std::filesystem::file_size(file_name) == minidb::PAGE_SIZE);
        minidb::page_id_t second_page_id = disk_manager.allocate_page();
        REQUIRE(second_page_id == 1);
        REQUIRE(std::filesystem::file_size(file_name) == minidb::PAGE_SIZE * 2);
    }
    std::filesystem::remove(file_name);
}
TEST_CASE("Allocated pages are zero initalized") {
    const std::string file_name = "file";
    std::filesystem::remove(file_name);
    {
    minidb::DiskManager disk_manager(file_name);
    minidb::page_id_t page_id = disk_manager.allocate_page();
    minidb::Page page;
    disk_manager.read_page(page_id, page);
    for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
        REQUIRE(page.data()[i] == std::byte{0});
    }
}
    std::filesystem::remove(file_name);
}

TEST_CASE("DiskManager read/write page works") {
    const std::string file_name = "file";
    std::filesystem::remove(file_name);
    {
        minidb::DiskManager disk_manager(file_name);
        minidb::page_id_t page_id = disk_manager.allocate_page();
        minidb::Page page_written;
        for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
            page_written.data()[i] = static_cast<std::byte>(i % 256);
        }
        disk_manager.write_page(page_id, page_written);
        REQUIRE(std::filesystem::file_size(file_name) == minidb::PAGE_SIZE);
        minidb::Page page_read;
        disk_manager.read_page(page_id, page_read);
        for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
            REQUIRE(page_read.data()[i] == page_written.data()[i]);
        }
    }
    std::filesystem::remove(file_name);
}

TEST_CASE("One page does not influence another") {
    const std::string file_name = "file";
    std::filesystem::remove(file_name);
    {
    minidb::DiskManager disk_manager(file_name);
    minidb::page_id_t page_id0 = disk_manager.allocate_page();
    minidb::Page page_0;
    minidb::page_id_t page_id1 = disk_manager.allocate_page();
    minidb::Page page_1;
    for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
        page_0.data()[i] = std::byte{0XAA};
        page_1.data()[i] = std::byte{0XAB};
    }
    disk_manager.write_page(page_id0, page_0);
    disk_manager.write_page(page_id1, page_1);
    minidb::Page page_read0;
    disk_manager.read_page(page_id0, page_read0);
    minidb::Page page_read1;
    disk_manager.read_page(page_id1, page_read1);
    for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
        REQUIRE(page_read0.data()[i] == std::byte{0XAA});
        REQUIRE(page_read1.data()[i] == std::byte{0XAB});
    }
    for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
        page_0.data()[i] = std::byte{0XAC};
    }
    disk_manager.write_page(page_id0, page_0);
    disk_manager.read_page(page_id0, page_read0);
    for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
        REQUIRE(page_read0.data()[i] == std::byte{0XAC});
    }
    disk_manager.read_page(page_id1, page_read1);
    for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
        REQUIRE(page_read1.data()[i] == std::byte{0XAB});
    }
}
    std::filesystem::remove(file_name);
}

TEST_CASE("DiskManager rejects invalid page IDs") {
    const std::string file_name = "file";
    std::filesystem::remove(file_name);
    {
        minidb::DiskManager disk_manager(file_name);
        minidb::Page page;
        REQUIRE_THROWS_AS(disk_manager.read_page(0, page), std::out_of_range);
        REQUIRE_THROWS_AS(disk_manager.write_page(0, page), std::out_of_range);
        minidb::page_id_t page_id = disk_manager.allocate_page();
        REQUIRE(page_id == 0);
        REQUIRE_THROWS_AS(disk_manager.read_page(1, page), std::out_of_range);
        REQUIRE_THROWS_AS(disk_manager.write_page(1, page), std::out_of_range);
        REQUIRE(std::filesystem::file_size(file_name) == minidb::PAGE_SIZE);
        for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
            page.data()[i] = std::byte{0XAA};
        }
        disk_manager.write_page(page_id, page);
        REQUIRE_THROWS_AS(disk_manager.write_page(1, page) , std::out_of_range);
        REQUIRE_THROWS_AS(disk_manager.read_page(1, page), std::out_of_range);
        minidb::Page page_read;
        disk_manager.read_page(page_id, page_read);
        for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
            REQUIRE(page_read.data()[i] == std::byte{0XAA});
        }
        REQUIRE(std::filesystem::file_size(file_name) == minidb::PAGE_SIZE);
    }
    std::filesystem::remove(file_name);
}

TEST_CASE("DiskManager persists data across restarts") {
    const std::string file_name = "file";
    std::filesystem::remove(file_name);
    {
        minidb::DiskManager disk_manager(file_name);
        minidb::page_id_t page_id = disk_manager.allocate_page();
        REQUIRE(page_id == 0);
        minidb::Page page;
        for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
            page.data()[i] = std::byte{0XAA};
        }
        disk_manager.write_page(page_id, page);
    }
    {
        minidb::DiskManager disk_manager(file_name);
        minidb::Page page_read;
        disk_manager.read_page(0, page_read);
        for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
            REQUIRE(page_read.data()[i] == std::byte{0XAA});
        }
        REQUIRE(std::filesystem::file_size(file_name) == minidb::PAGE_SIZE);
    }
    std::filesystem::remove(file_name);
}

TEST_CASE("DiskManager persists allocations across restarts") {
    const std::string file_name = "file";
    std::filesystem::remove(file_name);
    {
        minidb::DiskManager disk_manager(file_name);
        minidb::page_id_t page_id0 = disk_manager.allocate_page();
        REQUIRE(page_id0 == 0);
        minidb::page_id_t page_id1 = disk_manager.allocate_page();
        REQUIRE(page_id1 == 1);
    }
    {
        minidb::DiskManager disk_manager(file_name);
        minidb::page_id_t page_id2 = disk_manager.allocate_page();
        REQUIRE(page_id2 == 2);
        REQUIRE(std::filesystem::file_size(file_name) == minidb::PAGE_SIZE * 3);
    }
    std::filesystem::remove(file_name);
}

TEST_CASE("DiskManager rejects corrupted database files") {
    const std::string file_name = "file";
    std::filesystem::remove(file_name);
    {
        minidb::DiskManager disk_manager(file_name);
        minidb::page_id_t page_id = disk_manager.allocate_page();
        REQUIRE(page_id == 0);
        minidb::Page page;
        for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
            page.data()[i] = std::byte{0XAA};
        }
        disk_manager.write_page(page_id, page);
    }
    {
        std::ofstream file(file_name, std::ios::binary | std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for corruption");
        }
        std::string corrupted_data(100, 'a');
        file.write(corrupted_data.c_str(), corrupted_data.size());
    }
    REQUIRE(std::filesystem::file_size(file_name) == minidb::PAGE_SIZE + 100);
    REQUIRE_THROWS_AS(minidb::DiskManager(file_name), std::runtime_error);
    REQUIRE(std::filesystem::file_size(file_name) == minidb::PAGE_SIZE + 100);
    std::filesystem::remove(file_name);
}

TEST_CASE("DiskManager handles repeated I/O") {
    const std::string file_name = "file";
    std::filesystem::remove(file_name);
    {
        minidb::DiskManager disk_manager(file_name);
        minidb::page_id_t page_id_0 = disk_manager.allocate_page();
        minidb::page_id_t page_id_1 = disk_manager.allocate_page();
        minidb::page_id_t page_id_2 = disk_manager.allocate_page();
        REQUIRE(page_id_0 == 0);
        REQUIRE(page_id_1 == 1);
        REQUIRE(page_id_2 == 2);
        minidb::Page page_0;
        minidb::Page page_1;
        minidb::Page page_2;
        for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
            page_0.data()[i] = std::byte{0XAA};
            page_1.data()[i] = std::byte{0XAB};
            page_2.data()[i] = std::byte{0XAC}; 
        }
        disk_manager.write_page(page_id_0, page_0);
        disk_manager.write_page(page_id_1, page_1);
        disk_manager.write_page(page_id_2, page_2);
        minidb::Page page_read_0;
        minidb::Page page_read_1;
        minidb::Page page_read_2;
        disk_manager.read_page(page_id_2, page_read_2);
        disk_manager.read_page(page_id_0, page_read_0);
        disk_manager.read_page(page_id_1, page_read_1);
        for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
            REQUIRE(page_read_0.data()[i] == std::byte{0XAA});
            REQUIRE(page_read_1.data()[i] == std::byte{0XAB});
            REQUIRE(page_read_2.data()[i] == std::byte{0XAC});
        }
        for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
            page_1.data()[i] = std::byte{0XAD};
        }
        disk_manager.write_page(page_id_1, page_1);
        disk_manager.read_page(page_id_0, page_read_0);
        disk_manager.read_page(page_id_1, page_read_1);
        disk_manager.read_page(page_id_2, page_read_2);
        for (std::size_t i = 0; i < minidb::PAGE_SIZE; i++) {
            REQUIRE(page_read_0.data()[i] == std::byte{0XAA});
            REQUIRE(page_read_1.data()[i] == std::byte{0XAD});
            REQUIRE(page_read_2.data()[i] == std::byte{0XAC});
        }
        REQUIRE(std::filesystem::file_size(file_name) == minidb::PAGE_SIZE * 3);
    }
    std::filesystem::remove(file_name);
}