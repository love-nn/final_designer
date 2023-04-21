cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(final_designer)
include(cmake/utils.cmake)
set(CMAKE_VERBOSE_MAKEFILE ON) 
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

# find_package(Boost REQUIRED COMPONENTS
# boost_lexical_castConfig
# )

find_package(YAML-CPP REQUIRED COMPONENTS
)


set(LIB_SRC
    nnysl/log.cpp
    nnysl/util.cpp
    nnysl/config.cpp
    )


add_library(nnysl SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(nnysl) #__FILE__

add_definitions(-Wno-builtin-macro-redefined)
# add_library(nnysl_static STATIC ${LIB_SRC})
# SET_TARGET_PROPERTIES(nnysl_static PROPERTIES OUTPUT_NAME "nnysl")
# add_executable(test test/test.cpp)
# add_dependencies(test nnysl)
# target_link_libraries(test nnysl)

add_executable(test_config test/test_config.cpp)
add_dependencies(test_config nnysl)
force_redefine_file_macro_for_sources(test_config) #__FILE__

target_link_libraries(test_config nnysl yaml-cpp)




set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)