cmake_minimum_required(VERSION 3.20)

project(MiniDB LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(minidb
    src/main.cpp
)

target_compile_options(minidb PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
enable_testing()

add_executable(minidb_tests
    tests/common/test_tester.cpp
    tests/storage/page_test.cpp
    src/disk_manager.cpp
    tests/storage/disk_manager_test.cpp
)
target_include_directories(minidb_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(minidb PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(minidb_tests PRIVATE
    Catch2::Catch2WithMain
)
target_compile_options(minidb_tests PRIVATE
    -Wall
    -Wextra
    -Werror
    -pedantic
)
catch_discover_tests(minidb_tests DISCOVERY_MODE PRE_TEST)


option(MINIDB_ENABLE_SANITIZERS
    "Enable AddressSanitizer and UndefinedBehaviorSanitizer"
    ON
)

if(MINIDB_ENABLE_SANITIZERS)
    target_compile_options(minidb PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -g
    )

    target_link_options(minidb PRIVATE
        -fsanitize=address,undefined
    )

    target_compile_options(minidb_tests PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -g
    )

    target_link_options(minidb_tests PRIVATE
        -fsanitize=address,undefined
    )
endif()