include(FetchContent)

#source files
set(PROJECT_SOURCES
	"src/assembler.cpp"
)
set(PROJECT_HEADERS
	"include/cpu3026/assembler/assembler.h"
)

# add the library
add_library(cpu3026_assembler STATIC ${PROJECT_SOURCES} ${PROJECT_HEADERS})
add_library(cpu3026::assembler ALIAS cpu3026_assembler)
target_compile_features(cpu3026_assembler PRIVATE cxx_std_20)
target_link_libraries(cpu3026_assembler PUBLIC cpu3026::core)

# adds a directory to search when finding header files
target_include_directories(cpu3026_assembler
	PUBLIC "include"
)

# dependencies
set(BUILD_SHARED_LIBS ON)
