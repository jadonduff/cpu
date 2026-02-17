#assumes include(FetchContent)

message("importing pybind11...")
FetchContent_Declare(
  pybind11
  URL https://github.com/pybind/pybind11/archive/refs/tags/v3.0.1.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(pybind11)
message("pybind11 found!")
