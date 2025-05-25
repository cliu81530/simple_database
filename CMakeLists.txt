cmake_minimum_required(VERSION 3.30)
project(projectDB)

set(CMAKE_CXX_STANDARD 20)

add_executable(projectDB main.cpp
        Column.cpp
        Row.h
        Row.cpp
        Table.h
        Table.cpp
        Database.h
        Database.cpp
        QueryParser.h
        QueryParser.cpp)
