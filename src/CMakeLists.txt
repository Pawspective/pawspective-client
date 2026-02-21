add_subdirectory(models)
add_subdirectory(services)
add_subdirectory(utils)
add_subdirectory(state)
add_subdirectory(viewmodels)



add_library(pawspective_src INTERFACE)

target_link_libraries(pawspective_src INTERFACE 
    pawspective_utils
    pawspective_models
    pawspective_services
    pawspective_state
    pawspective_viewmodels
)

target_include_directories(pawspective_src INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})