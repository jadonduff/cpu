include(FetchContent)

#source files
set(PROJECT_SOURCES
	"src/source.cpp"
)
set(PROJECT_HEADERS
	"include/cpu3026/cli/command_handler.h"
	"include/cpu3026/cli/commands.h"
	"include/cpu3026/cli/io.h"
 "include/cpu3026/cli/commands/load_cmd.h")

# add the library
add_library(cpu3026_cli STATIC ${PROJECT_SOURCES} ${PROJECT_HEADERS})
add_library(cpu3026::cli ALIAS cpu3026_cli)
target_compile_features(cpu3026_cli PRIVATE cxx_std_20)
target_link_libraries(cpu3026_cli PUBLIC cpu3026::core cpu3026::vm)

# adds a directory to search when finding header files
target_include_directories(cpu3026_cli 
	PUBLIC "include" 
)

# dependencies
set(BUILD_SHARED_LIBS ON)
