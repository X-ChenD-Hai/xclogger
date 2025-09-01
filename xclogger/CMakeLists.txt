add_library(xclogger INTERFACE)
add_library(zmq_support INTERFACE)
target_include_directories(
    xclogger INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(zmq_support INTERFACE cppzmq-static xclogger)
add_library(xclogger::xclogger ALIAS xclogger)
add_library(xclogger::zmq_support ALIAS zmq_support)

file(GLOB headers CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

# Install headers to xclogger subdirectory
install(FILES ${headers}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xclogger
)

# Install the library targets
install(TARGETS xclogger zmq_support
    EXPORT xcloggerTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_ARCHIVEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_RUNTIMEDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Create and install package configuration files
install(EXPORT xcloggerTargets
    FILE xcloggerTargets.cmake
    NAMESPACE xclogger::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xclogger
)

