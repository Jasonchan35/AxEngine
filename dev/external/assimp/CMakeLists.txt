if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-Wno-error=character-conversion
						-Wno-character-conversion)
endif()

set(ASSIMP_BUILD_SAMPLES OFF)
set(ASSIMP_BUILD_TEST    OFF)
set(ASSIMP_BUILD_DOCS    OFF)


add_subdirectory(assimp-master)
set_target_properties (assimp PROPERTIES FOLDER "external")
