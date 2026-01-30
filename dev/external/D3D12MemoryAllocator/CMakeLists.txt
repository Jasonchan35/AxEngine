set(PROJECT_NAME D3D12MemoryAllocator)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC)
file(GLOB_RECURSE all_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

target_include_directories(${PROJECT_NAME} PUBLIC "src/D3D12MemoryAllocator")

target_sources(${PROJECT_NAME} PRIVATE ${all_files})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "external")

target_link_libraries(${PROJECT_NAME} PUBLIC DirectX-Headers)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(${PROJECT_NAME} PRIVATE 
			-Werror
			-Wno-switch)
endif()