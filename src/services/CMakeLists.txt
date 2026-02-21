set(UTILS_SOURCES 
)

add_library(pawspective_services STATIC ${UTILS_SOURCES})

target_include_directories(pawspective_services PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_services
    PUBLIC
)

if(BUILD_TESTING)
    add_executable(pawspective_services_tests)

    target_link_libraries(pawspective_services_tests
        PRIVATE
        pawspective_services
    )

    add_test(NAME pawspective_services_tests COMMAND pawspective_services_tests)
endif()