cmake_minimum_required(VERSION 3.28)  # 3.26+ works, but 3.28+ has better module support
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/_vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Require C++20 (or C++23 for better module support)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# IMPORTANT: Enable experimental module support (CMake 3.28+)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP ON)

# Tell CMake where to put module files (.pcm, .ifc, etc.)
set(CMAKE_CXX_MODULE_STD ON)  # Use standard module behavior when possible

include(ax_common.cmake)

set(PROJECT_NAME "AxEngine")

if (CMAKE_GENERATOR_TOOLSET)
	set(PROJECT_NAME "${PROJECT_NAME}-${CMAKE_GENERATOR_TOOLSET}")
endif()
if (CMAKE_GENERATOR_PLATFORM)
	set(PROJECT_NAME "${PROJECT_NAME}-${CMAKE_GENERATOR_PLATFORM}")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

message("==== ${PROJECT_NAME} ====")
message("VCPKG_TARGET_TRIPLET           = ${VCPKG_TARGET_TRIPLET}")
message("CMAKE_CURRENT_BINARY_DIR       = ${CMAKE_CURRENT_BINARY_DIR}")
message("CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")


if(CMAKE_GENERATOR STREQUAL Xcode)
	project(${PROJECT_NAME} LANGUAGES C CXX OBJC OBJCXX VERSION ${PROJECT_VER})
else()
	project(${PROJECT_NAME} LANGUAGES C CXX)
endif()

project(${PROJECT_NAME} VERSION 0.1.0)

add_subdirectory(libax8/AxCore)
add_subdirectory(libax8/AxCore/test)

add_subdirectory(libax8/AxNativeUI)
add_subdirectory(libax8/AxRender)
add_subdirectory(libax8/AxUI)

add_subdirectory(libax8/Tools/AxFileTool)
add_subdirectory(libax8/Tools/AxCppHeaderTool)
add_subdirectory(libax8/Tools/AxShaderTool)

add_subdirectory(libax8/AxEngine)
add_subdirectory(libax8/AxEditor)

add_subdirectory(AxEditorApp)

# target_sources(${PROJECT_NAME} PRIVATE ax_common.cmake)

# add_executable(myapp)
# 
# target_sources(myapp
#   PUBLIC
#     FILE_SET cxx_modules TYPE CXX_MODULES FILES
#       src/greetings.cppm
#       src/greetings.cpp
#       src/math.ixx
#   PRIVATE
#     src/main.cpp
# )
# 
# # Optional: Enable warnings
# target_compile_options(myapp PRIVATE
#     $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
#     $<$<CXX_COMPILER_ID:MSVC>:/W4>
# )