set(UTILS_SOURCES 
)

add_library(pawspective_state STATIC ${UTILS_SOURCES})

target_include_directories(pawspective_state PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_state
    PUBLIC
)

if(BUILD_TESTING)
    add_executable(pawspective_state_tests)

    target_link_libraries(pawspective_state_tests
        PRIVATE
        pawspective_state
    )

    add_test(NAME pawspective_state_tests COMMAND pawspective_state_tests)
endif()