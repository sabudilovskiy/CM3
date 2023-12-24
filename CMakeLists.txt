cmake_minimum_required(VERSION 3.27)
project(CM3)

set(CMAKE_CXX_STANDARD 20)


add_executable(CM3 main.cpp
        src/token_type.hpp
        src/parser.hpp
        src/constexpr_string.hpp
        src/ast_builder.hpp)
target_include_directories(CM3 PRIVATE src)