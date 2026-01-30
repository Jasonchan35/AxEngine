set(PROJECT_NAME DirectX-Headers)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

set(DxIncludeDir "${CMAKE_CURRENT_SOURCE_DIR}/DirectX-Headers-main/include")

add_library(${PROJECT_NAME} INTERFACE)
file(GLOB_RECURSE all_files "${DxIncludeDir}/*.h")

target_sources(${PROJECT_NAME} PUBLIC ${all_files})


target_include_directories(${PROJECT_NAME} INTERFACE ${DxIncludeDir})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "external")
