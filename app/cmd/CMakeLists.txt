cmake_minimum_required(VERSION 3.16)
include(FetchContent)

#source files
set(PROJECT_SOURCES
	"src/source.cpp"
)
set(PROJECT_HEADERS
)

# add the exectuable
add_executable(cpu3026_cmd ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_compile_features(cpu3026_cmd PRIVATE cxx_std_20)
target_link_libraries(cpu3026_cmd cpu3026::cpu3026)

# dependencies
set(BUILD_SHARED_LIBS ON)
