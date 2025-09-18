cmake_minimum_required(VERSION 3.15)
project(logger VERSION 1.0.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
find_package(cppzmq CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
add_subdirectory(xclogger)
option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/xcloggerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/xcloggerConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xclogger
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/xcloggerConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/xcloggerConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/xcloggerConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xclogger
)
