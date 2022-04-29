cmake_minimum_required(VERSION 3.0.0)
project(interview VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)
 
add_executable(${PROJECT_NAME} Test.cpp)
 
target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES})

enable_testing()

# This is so you can do 'make test' to see all your tests run, instead of
# manually running the executable runUnitTests to see those specific tests.
add_test(NAME test COMMAND ${PROJECT_NAME})