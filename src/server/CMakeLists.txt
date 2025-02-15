set(APP_SOURCES        
        auth.cpp        
        server.cpp
)
add_executable(calc_server  ${APP_SOURCES})

target_link_libraries(calc_server pthread)  
