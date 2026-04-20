include(FetchContent)

#source files
set(PROJECT_SOURCES
	"src/source.cpp"
)
set(PROJECT_HEADERS
	"include/cpu3026/rest/rest.h"
)

# add the library
add_library(cpu3026_rest STATIC ${PROJECT_SOURCES} ${PROJECT_HEADERS})
add_library(cpu3026::rest ALIAS cpu3026_rest)
target_compile_features(cpu3026_rest PRIVATE cxx_std_20)
target_link_libraries(cpu3026_rest PUBLIC cpu3026::core cpu3026::vm)

# adds a directory to search when finding header files
target_include_directories(cpu3026_rest 
	PUBLIC "include" 
)

# dependencies
set(BUILD_SHARED_LIBS ON)

include(boost)
target_link_libraries(cpu3026_rest PUBLIC Boost::asio Boost::beast)
