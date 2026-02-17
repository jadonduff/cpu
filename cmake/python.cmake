#assumes include(FetchContent)

message("importing python...")
find_package(Python COMPONENTS Interpreter Development REQUIRED)
message("python found!")
