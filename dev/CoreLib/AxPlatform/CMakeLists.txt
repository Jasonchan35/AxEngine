set(PROJECT_NAME AxPlatform)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "CoreLib/AxPlatform")

# target_link_libraries(${PROJECT_NAME} PUBLIC AxDetectPlatform)

