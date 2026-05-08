cmake_minimum_required(VERSION 3.12.0)
project(nmpde-exam LANGUAGES CXX C)

include(./cmake-common.cmake)

add_executable(q1 src/main.cpp src/q1.cpp)
deal_ii_setup_target(q1)