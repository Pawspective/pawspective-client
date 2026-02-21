set(UTILS_SOURCES 
)

add_library(pawspective_models STATIC ${UTILS_SOURCES})

target_include_directories(pawspective_models PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_models
    PUBLIC
)

if(BUILD_TESTING)
    add_executable(pawspective_models_tests)

    target_link_libraries(pawspective_models_tests
        PRIVATE
        pawspective_models
    )

    add_test(NAME pawspective_models_tests COMMAND pawspective_models_tests)
endif()