set(PROJECT_NAME AxEditor)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "libax8")

target_link_libraries(${PROJECT_NAME} PUBLIC AxEngine AxUI)
