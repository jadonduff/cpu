cmake_minimum_required(VERSION 3.16)

# set the project name
project(cpu3026 VERSION 0.1.0 LANGUAGES C CXX)

#options
option(CPU3026_PYBIND "generate pybind library" OFF)


#config
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# include libraries
add_subdirectory(lib/core)
add_subdirectory(lib/memory)
add_subdirectory(lib/processor)

# add global library
add_library(cpu3026 INTERFACE)
add_library(cpu3026::cpu3026 ALIAS cpu3026)
target_link_libraries(cpu3026 INTERFACE cpu3026::core cpu3026::memory cpu3026::processor)

#add executables
add_subdirectory(app/cmd)
add_subdirectory(test)
if(CPU3026_PYBIND)
    add_subdirectory(lib/cpu3026_py)
    add_subdirectory(app/python)
endif()

#project warnings
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()