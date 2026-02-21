set(UTILS_SOURCES 
    src/validator.cpp
)

add_library(pawspective_utils STATIC ${UTILS_SOURCES})

target_include_directories(pawspective_utils PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_utils
    PUBLIC
)

if(BUILD_TESTING)
    add_executable(pawspective_utils_tests
    )

    target_link_libraries(pawspective_utils_tests
        PRIVATE
        pawspective_utils
    )

    add_test(NAME pawspective_utils_tests COMMAND pawspective_utils_tests)
endif()