set(PROJECT_NAME AxEditor)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "AxEngine")
ax_cpp_header_tool(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(pxr REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC AxEngine AxImUI usd)

