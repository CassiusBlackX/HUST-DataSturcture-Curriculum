cmake_minimum_required(VERSION 3.24)
project(DSEA2)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(DSEA2 main.cpp src/lexer.cpp src/parser.cpp src/preprocess.cpp src/printfile.cpp src/profunction.cpp src/SymbolTable.cpp)
