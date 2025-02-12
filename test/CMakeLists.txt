set(TEST_SOURCE
    test_calculator.cpp
    test_server.cpp
)

add_executable(calc_tests ${TEST_SOURCES})

target_link_libraries(calc_tests pthread)  
