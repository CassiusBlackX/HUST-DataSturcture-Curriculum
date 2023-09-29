cmake_minimum_required(VERSION 3.24)
project(DSEA2)

set(CMAKE_CXX_STANDARD 17)

add_executable(DSEA2 main.cpp lexer.cpp parser.cpp preprocess.cpp printfile.cpp profunction.cpp SymbolTable.cpp)
