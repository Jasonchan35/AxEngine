set(PROJECT_NAME AxEditor)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "AxEngine")
ax_cpp_header_tool(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PUBLIC AxEngine)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)
# target_link_libraries(${PROJECT_NAME} PUBLIC OpenUSD-prebuilt)

# find_package(pxr CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PUBLIC usd)

# Easy solution: copy all directories from "/lib/usd" of OpenUSD installed by vcpkg to the "/lib/usd" of your application.
# if(WIN32)
# add_custom_command(TARGET ${PROJECT_NAME}
# 	POST_BUILD
# 	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_newer "${AX_PROJECT_ROOT}/_vcpkg/installed/x64-windows/debug/plugin/usd" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
# 	VERBATIM
# )
# endif()
