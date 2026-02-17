cmake_minimum_required(VERSION 3.16)
include(FetchContent)

#source files
set(PROJECT_SOURCES
	"src/source.cpp"
)
set(PROJECT_HEADERS
)



# dependencies
set(BUILD_SHARED_LIBS ON)

include(python)
include(pybind11)

# add the library
pybind11_add_module(cpu3026_py ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_compile_features(cpu3026_py PRIVATE cxx_std_20)
target_link_libraries(cpu3026_py PUBLIC cpu3026::cpu3026)
