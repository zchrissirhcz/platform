cmake_minimum_required(VERSION 3.16)
project(platform VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(platform INTERFACE)
target_include_directories(platform INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(platform INTERFACE cxx_std_11)

enable_testing()

set(PLATFORM_TESTS os compiler arch simd)
foreach(name IN LISTS PLATFORM_TESTS)
    add_executable(test_${name} tests/test_${name}.cpp)
    target_link_libraries(test_${name} PRIVATE platform)
    if(MSVC)
        target_compile_options(test_${name} PRIVATE /W4)
    else()
        target_compile_options(test_${name} PRIVATE -Wall -Wextra -pedantic)
    endif()
    add_test(NAME ${name} COMMAND test_${name})
endforeach()

install(TARGETS platform EXPORT platform-targets)
install(DIRECTORY include/platform DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/platform)
install(EXPORT platform-targets
    NAMESPACE platform::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/platform)

configure_package_config_file(
    cmake/platformConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/platformConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/platform)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/platformConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/platformConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/platformConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/platform)
