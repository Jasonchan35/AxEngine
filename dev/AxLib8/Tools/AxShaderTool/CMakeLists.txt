set(PROJECT_NAME AxShaderTool)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "AxLib8_Tools")

target_link_libraries(${PROJECT_NAME} PUBLIC AxRender)


if(WIN32)
	set(vulkan_sdk_path $ENV{VK_SDK_PATH})
	target_include_directories(${PROJECT_NAME} PUBLIC "${vulkan_sdk_path}/Source/SPIRV-Reflect")
	target_link_directories(   ${PROJECT_NAME} PUBLIC "${vulkan_sdk_path}/Lib")
endif()