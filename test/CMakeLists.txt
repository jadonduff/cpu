cmake_minimum_required(VERSION 3.16)
include(FetchContent)

#source files
set(PROJECT_SOURCES
	"src/source.cpp"
)
set(PROJECT_HEADERS
 "src/core_tests.h" "src/memory_tests.h" "src/processor_tests.h")

# add the executable
add_executable(cpu3026_test ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_compile_features(cpu3026_test PRIVATE cxx_std_20)
target_link_libraries(cpu3026_test cpu3026::cpu3026)

# dependencies
set(BUILD_SHARED_LIBS ON)

include(gtest)
target_link_libraries(cpu3026_test GTest::gtest_main)


#google test required lines
include(GoogleTest)
gtest_discover_tests(cpu3026_test)