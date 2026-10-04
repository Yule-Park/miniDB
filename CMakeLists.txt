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

FetchContent_MakeAvailable(Catch2)
enable_testing()
add_executable(minidb_tests
    tests/common/test_tester.cpp
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
add_test(
    NAME minidb_tests
    COMMAND minidb_tests
)

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