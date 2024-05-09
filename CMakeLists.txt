cmake_minimum_required(VERSION 3.16)
project(bpt)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(bpt
              main.cpp
              bptree.hpp
              vector.hpp
              exceptions.hpp
              file.hpp
              utils.hpp)