if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-Wno-error=character-conversion
						-Wno-character-conversion)
endif()

add_subdirectory(assimp-master)
set_target_properties (assimp PROPERTIES FOLDER "external")
