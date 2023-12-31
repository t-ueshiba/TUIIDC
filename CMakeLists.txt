cmake_minimum_required(VERSION 3.5)
project(TUIIDC)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif()

enable_language(CXX)
set(CMAKE_CXX_STANDARD		17)
set(CMAKE_CXX_STANDARD_REQUIRED	ON)
set(CMAKE_CXX_EXTENSIONS	OFF)
set(CMAKE_CXX_FLAGS_RELEASE	"-Wall -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG	"-g")
add_definitions(-DTUIIDCPP_CONF_DIR="${CMAKE_INSTALL_PREFIX}/etc")

if(APPLE)
  set(MACOSX_RPATH TRUE)
endif()

find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED)
pkg_check_modules(LIBUSB-1.0 REQUIRED libusb-1.0)
pkg_check_modules(YAML-CPP REQUIRED yaml-cpp)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_INSTALL_PREFIX}/include
    ${Boost_INCLUDE_DIRS}
    ${LIBUSB-1.0_INCLUDE_DIRS}
    ${YAML-CPP_INCLUDE_DIRS}
)

link_directories(
    ${CMAKE_INSTALL_PREFIX}/lib
    ${LIBUSB-1.0_LIBRARY_DIRS}
    ${YAML-CPP_LIBRARY_DIRS}
)

file(GLOB sources *.cc)
add_library(${PROJECT_NAME} SHARED ${sources})

if(APPLE)
    add_subdirectory(MacOSX)
    include_directories(MacOSX)
    target_link_libraries(${PROJECT_NAME}
			  TUTools raw1394
			  ${LIBUSB-1.0_LIBRARIES} ${YAML-CPP_LIBRARIES})
else()
    pkg_check_modules(LIBRAW1394 REQUIRED libraw1394)
    include_directories(${LIBRAW1394_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}
			  TUTools ${LIBRAW1394_LIBRARIES}
			  ${LIBUSB-1.0_LIBRARIES} ${YAML-CPP_LIBRARIES})
endif()

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)

add_subdirectory(TU)
add_subdirectory(testIIDCcamera)

option(BUILD_TUIIDCPP_TEST_PROGRAMS "Building test programs for TUIIDC" OFF)

if(BUILD_TUIIDCPP_TEST_PROGRAMS)
  add_subdirectory(testcam)
  add_subdirectory(testcam2)
endif()
