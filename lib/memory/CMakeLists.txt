include(FetchContent)

#source files
set(PROJECT_SOURCES
	"src/source.cpp"
	"src/memory.cpp"
	"src/io_memory.cpp"
)
set(PROJECT_HEADERS
	"include/cpu3026/memory/memory.h"
	"include/cpu3026/memory/io_memory.h"
)

# add the library
add_library(cpu3026_memory STATIC ${PROJECT_SOURCES} ${PROJECT_HEADERS})
add_library(cpu3026::memory ALIAS cpu3026_memory)
target_compile_features(cpu3026_memory PRIVATE cxx_std_20)
target_link_libraries(cpu3026_memory PUBLIC cpu3026::core)

# adds a directory to search when finding header files
target_include_directories(cpu3026_memory 
	PUBLIC "include" 
)

# dependencies
set(BUILD_SHARED_LIBS ON)
