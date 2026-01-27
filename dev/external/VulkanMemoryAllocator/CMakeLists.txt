set(PROJECT_NAME VulkanMemoryAllocator)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC)
file(GLOB_RECURSE all_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

target_include_directories(${PROJECT_NAME} PUBLIC src)
target_sources(${PROJECT_NAME} PRIVATE ${all_files})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "external")

if(WIN32)
	set(vulkan_sdk_path $ENV{VK_SDK_PATH})
	target_include_directories(${PROJECT_NAME} PUBLIC "${vulkan_sdk_path}/Include")
	target_link_directories(   ${PROJECT_NAME} PUBLIC "${vulkan_sdk_path}/Lib")
	target_link_libraries(${PROJECT_NAME} PUBLIC vulkan-1)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(${PROJECT_NAME} PRIVATE -Wno-nullability-completeness)
endif()
