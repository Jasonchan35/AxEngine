set(PROJECT_NAME AxCore_Test)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_gen_test_all_cpp(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

ax_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "AxLib8")
target_link_libraries(${PROJECT_NAME} PUBLIC AxCore)