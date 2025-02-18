set(APP_SOURCES        
        client.cpp
)
add_executable(calc_client ${APP_SOURCES})

target_link_libraries(calc_client pthread)  
