cmake_minimum_required(VERSION 2.8.4)
project(Zadanie_4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

set(SOURCE_FILES_zad4
    ${CMAKE_CURRENT_LIST_DIR}/main.c)

add_executable(sasm ${SOURCE_FILES_zad4})