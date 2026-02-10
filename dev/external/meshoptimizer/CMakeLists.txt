
add_subdirectory(meshoptimizer-master)


if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
set_target_properties (meshoptimizer PROPERTIES FOLDER "external")
endif()