set(UTILS_SOURCES 
)

add_library(pawspective_viewmodels STATIC ${UTILS_SOURCES})

target_include_directories(pawspective_viewmodels PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_viewmodels
    PUBLIC
)

if(BUILD_TESTING)
    add_executable(pawspective_viewmodels_tests)

    target_link_libraries(pawspective_viewmodels_tests
        PRIVATE
        pawspective_viewmodels
    )

    add_test(NAME pawspective_viewmodels_tests COMMAND pawspective_viewmodels_tests)
endif()