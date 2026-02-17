set(PROJECT_SOURCES
	"main.py"
)

include(python)

if(WIN32)
    configure_file(
        "config.cmd.in"
        "config.cmd"
        @ONLY
    )
    add_custom_target(
        cpu3026_python
        COMMAND "${CMAKE_CURRENT_BINARY_DIR}/config.cmd"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Running the generated configuration script for python sub-library..."
        VERBATIM
        USES_TERMINAL
        SOURCES ${PROJECT_SOURCES}
    )
elseif(APPLE)

else() # linux
    configure_file(
        "config.sh.in"
        "config.sh"
        @ONLY
    )
    add_custom_target(
        cpu3026_python
        COMMAND bash -c "${CMAKE_CURRENT_BINARY_DIR}/config.sh"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Running the generated configuration script for python sub-library..."
        VERBATIM
        USES_TERMINAL
        SOURCES ${PROJECT_SOURCES}
    )
endif()

if(Python_FOUND) 
    add_dependencies(cpu3026_python cpu3026_py)
endif()