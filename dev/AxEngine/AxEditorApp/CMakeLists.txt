set(PROJECT_NAME AxEditorApp)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "AxEngine")
target_link_libraries(${PROJECT_NAME} PUBLIC AxEditor)

# OpenUSD Debug
set_target_properties(${PROJECT_NAME} PROPERTIES
  VS_DEBUGGER_ENVIRONMENT "TF_DEBUG=*"
)
