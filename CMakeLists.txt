cmake_minimum_required(VERSION 3.10)
project(logger VERSION 1.0.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
find_package(cppzmq CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
add_subdirectory(src)
add_subdirectory(tests)
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
