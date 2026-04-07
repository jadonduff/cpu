message("importing boost...")

set(BOOST_INCLUDE_LIBRARIES "asio")


FetchContent_Declare(
	Boost
	URL https://github.com/boostorg/boost/releases/download/boost-1.90.0/boost-1.90.0-cmake.tar.xz
	URL_HASH SHA256=aca59f889f0f32028ad88ba6764582b63c916ce5f77b31289ad19421a96c555f
	DOWNLOAD_EXTRACT_TIMESTAMP ON
	FIND_PACKAGE_ARGS CONFIG
)

set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(Boost)
set(BUILD_SHARED_LIBS ON)

if(boost_POPULATED)
	message("boost found!")
else()
	message(SEND_ERROR "failed to find boost")
endif()

