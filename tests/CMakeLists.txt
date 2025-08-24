find_package(GTest CONFIG REQUIRED)

set(GTEST_LIBS GTest::gtest GTest::gtest_main)
set(GMOCK_LIBS GTest::gmock GTest::gmock_main)


add_executable(ipc ipc.cc)
target_link_libraries(ipc PRIVATE cpp-ipc::ipc)
add_executable(log log.cc)
target_link_libraries(log PRIVATE ${GTEST_LIBS})
add_executable(ipclog ipclog.cc)
target_link_libraries(ipclog PRIVATE ${GTEST_LIBS} cpp-ipc::ipc)
