find_package(GTest CONFIG REQUIRED)

set(GTEST_LIBS GTest::gtest GTest::gtest_main)
set(GMOCK_LIBS GTest::gmock GTest::gmock_main)


add_executable(zmq zmq.cc)
target_link_libraries(zmq PRIVATE zmq_support)
add_executable(log log.cc)
target_link_libraries(log PRIVATE ${GTEST_LIBS} xclogger)
add_executable(zmqlog zmqlog.cc)
target_link_libraries(zmqlog PRIVATE ${GTEST_LIBS} zmq_support)


