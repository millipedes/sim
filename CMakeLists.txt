cmake_minimum_required(VERSION 3.5)
project(sedim)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(tl-expected REQUIRED)
find_package(nlohmann_json REQUIRED)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/test")
set(SRC_FILES
  ${SRC_DIR}/Context.cpp
  ${SRC_DIR}/Parsing.cpp
)

include_directories(
  ${SRC_DIR}
)

add_executable(sedim ${SRC_FILES} src/main.cpp)

target_link_libraries(sedim
  tl::expected
  nlohmann_json::nlohmann_json
)

option(BUILD_TESTS "Build Test Suite" ON)

if(BUILD_TESTS)
  enable_testing()
  set(TEST_SRC_FILES
    ${TEST_DIR}/ParsingTest.cpp
    ${TEST_DIR}/ExecutionTest.cpp
  )
  add_executable(tests ${SRC_FILES} ${TEST_SRC_FILES} test/main.cpp)
  target_link_libraries(tests gtest gtest_main pthread)
endif()
