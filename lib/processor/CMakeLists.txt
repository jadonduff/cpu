cmake_minimum_required(VERSION 3.16)
include(FetchContent)

#source files
set(PROJECT_SOURCES
	"src/source.cpp"
	"include/cpu3026/processor/cpu3026.h"
)
set(PROJECT_HEADERS
)

# add the library
add_library(cpu3026_processor STATIC ${PROJECT_SOURCES} ${PROJECT_HEADERS})
add_library(cpu3026::processor ALIAS cpu3026_processor)
target_compile_features(cpu3026_processor PRIVATE cxx_std_20)
target_link_libraries(cpu3026_processor PUBLIC cpu3026::core)

# adds a directory to search when finding header files
target_include_directories(cpu3026_processor 
	PUBLIC "include" 
	PUBLIC "${cpu3026_BINARY_DIR}/core/include"
)

# dependencies
set(BUILD_SHARED_LIBS ON)
