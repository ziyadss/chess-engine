cmake_minimum_required(VERSION 3.23)

project(chess_engine VERSION 0.1)

configure_file(config.h.in config.h)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ERROR_DEPRECATED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(chess_engine main.cpp)

set(WARNINGS1 "-Wall;-Wpedantic;-Wextra;-Wshadow;-Wfloat-equal;-Wparentheses;-Wformat=2;-Wnoexcept;-Wredundant-tags;-Wuseless-cast;")
set(WARNINGS2 "-Wlogical-op;-Wshift-overflow=2;-Wduplicated-cond;-Wcast-qual;-Wcast-align;-Wsuggest-final-types;-Weffc++;")
set(WARNINGS3 "-Wsuggest-override;-Wstrict-null-sentinel;-Wold-style-cast;-Wzero-as-null-pointer-constant;-Wextra-semi;")

#set(FLAGS "-Ofast;")
set(OPTIMIZATIONS "-fstrict-enums")

target_compile_options(chess_engine PUBLIC ${WARNINGS1})
target_compile_options(chess_engine PUBLIC ${WARNINGS2})
target_compile_options(chess_engine PUBLIC ${WARNINGS3})
target_compile_options(chess_engine PUBLIC ${FLAGS})
target_compile_options(chess_engine PUBLIC ${OPTIMIZATIONS})

target_include_directories(chess_engine PUBLIC ${PROJECT_BINARY_DIR})
