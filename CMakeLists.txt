cmake_minimum_required(VERSION 3.17)
project(leg)

set(CMAKE_CXX_STANDARD 20)

message("Using ${CMAKE_BUILD_TYPE} build")

execute_process(
    COMMAND llvm-config --includedir
    OUTPUT_VARIABLE LLVM_INCLUDE_DIR
)
execute_process(
    COMMAND llvm-config --libdir
    OUTPUT_VARIABLE LLVM_LIB_DIR
)
execute_process(
    COMMAND llvm-config --libs --system-libs
    OUTPUT_VARIABLE LLVM_LIBS
)
execute_process(
    COMMAND llvm-config --cppflags
    OUTPUT_VARIABLE LLVM_CPP_FLAGS
)

string(REGEX REPLACE "\n$" "" LLVM_INCLUDE_DIR "${LLVM_INCLUDE_DIR}")
string(REGEX REPLACE "\n$" "" LLVM_LIB_DIR "${LLVM_LIB_DIR}")
string(REGEX REPLACE "\n$" "" LLVM_LIBS "${LLVM_LIBS}")
string(REGEX REPLACE "\n$" "" LLVM_CPP_FLAGS "${LLVM_CPP_FLAGS}")
separate_arguments(LLVM_LIBS_ARGS UNIX_COMMAND ${LLVM_LIBS})
separate_arguments(LLVM_CPP_FLAGS_ARGS UNIX_COMMAND ${LLVM_CPP_FLAGS})

message(STATUS "Loaded LLVM includes dir: ${LLVM_INCLUDE_DIR}")
message(STATUS "Loaded LLVM libs dir: ${LLVM_LIB_DIR}")
message(STATUS "Loaded LLVM cpp flags: ${LLVM_CPP_FLAGS}")

include(.conan/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(src/compiler)
include_directories(${LLVM_INCLUDE_DIR})

link_directories(${LLVM_LIB_DIR})

add_definitions(${LLVM_CPP_FLAGS_ARGS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

add_executable(leg_compiler src/compiler/main.cpp)
add_executable(leg_preprocessor src/preprocessor/main.cpp)
add_executable(leg_cli src/cli/main.cpp)

target_link_libraries(leg_compiler ${LLVM_LIBS_ARGS} ${CONAN_LIBS})
target_link_libraries(leg_cli ${CONAN_LIBS})
