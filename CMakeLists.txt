cmake_minimum_required(VERSION 3.17)
project(BASIC_lang)

set(CMAKE_CXX_STANDARD 14)

include_directories(BASIC_lang)

add_executable(BASIC_lang
        BASIC_lang/AST.h
        BASIC_lang/color.h
        BASIC_lang/common.h
        BASIC_lang/Error.cpp
        BASIC_lang/main.cpp
        BASIC_lang/main.h
        BASIC_lang/object.h
        BASIC_lang/ParserCore.cpp
        BASIC_lang/Program.cpp
        BASIC_lang/runner.cpp
        BASIC_lang/token.h
        BASIC_lang/tokenize.cpp
        BASIC_lang/types.cpp)
