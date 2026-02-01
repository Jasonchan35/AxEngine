set(PROJECT_NAME AxRender)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

ax_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "AxLib8")

file(GLOB hlsl_file_list LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/../../../deploy/Assets/Shaders/include/AxHLSL/*")
target_sources(${PROJECT_NAME} PUBLIC ${hlsl_file_list})

target_link_libraries(${PROJECT_NAME} PUBLIC AxNativeUI)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui-docking )
target_link_libraries(${PROJECT_NAME} PUBLIC DirectX-Headers D3D12MemoryAllocator VulkanMemoryAllocator)

#--------------------
find_package(PNG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG)

find_package(JPEG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC JPEG::JPEG)

find_package(Freetype REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Freetype::Freetype)

# find_package(D3D12MemoryAllocator CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE GPUOpen::D3D12MemoryAllocator)

# find_package(imgui CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)