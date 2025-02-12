set(APP_SOURCES        
        calculator.cpp
        auth.cpp        
        server.cpp
)
add_executable(calc_app  ${APP_SOURCES})

target_link_libraries(calc_app pthread)  
