include(FetchContent)

#source files
set(PROJECT_SOURCES
	"src/source.cpp"
)
set(PROJECT_HEADERS
	"include/cpu3026/vm/vm.h"
)

# add the library
add_library(cpu3026_vm STATIC ${PROJECT_SOURCES} ${PROJECT_HEADERS})
add_library(cpu3026::vm ALIAS cpu3026_vm)
target_compile_features(cpu3026_vm PRIVATE cxx_std_20)
target_link_libraries(cpu3026_vm PUBLIC cpu3026::core cpu3026::processor cpu3026::memory)

# adds a directory to search when finding header files
target_include_directories(cpu3026_vm 
	PUBLIC "include" 
)

# dependencies
set(BUILD_SHARED_LIBS ON)
