include_guard(GLOBAL)

message("CMAKE_SYSTEM_NAME         = ${CMAKE_SYSTEM_NAME}")
message("CMAKE_GENERATOR_PLATFORM  = ${CMAKE_GENERATOR_PLATFORM}")
message("CMAKE_GENERATOR_TOOLSET   = ${CMAKE_GENERATOR_TOOLSET}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#---- helper functions ----------

# function(ax_test_call_by_reference varName)
#   SET(${varName} "value to return" PARENT_SCOPE)
# endfunction()

function(ax_write_to_file filename text)
#	message("ax_write_to_file(\"${filename}\")")
	file(GENERATE OUTPUT ${filename} CONTENT "${text}" NEWLINE_STYLE UNIX)	
endfunction()

function(ax_add_all_subdirectory_recurse src_path)
	file(GLOB_RECURSE V_GLOB LIST_DIRECTORIES true "${src_path}/*")
	foreach(item ${V_GLOB})
		if(IS_DIRECTORY ${item})
			if (EXISTS "${item}/CMakeLists.txt")
				# message("add_subdirectory(" ${item} ")")
				add_subdirectory(${item})
			endif()
		endif()
	endforeach()
endfunction()

function(ax_dump_cmake_variables)
	get_cmake_property(_variableNames VARIABLES)
	list (SORT _variableNames)
	foreach (_variableName ${_variableNames})
		if (ARGV0)
			unset(MATCHED)
			string(REGEX MATCH ${ARGV0} MATCHED ${_variableName})
			if (NOT MATCHED)
				continue()
			endif()
		endif()
		message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()
endfunction()

function(ax_set_warning_level target_name)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		# message("ax_set_warning_level MSVC")
		target_compile_options(${target_name} PRIVATE /utf-8) 	# execution_character_set - otherwise std::format wouldn't check format in compile time for "char"
		target_compile_options(${target_name} PRIVATE /WX)    	#warning treated as error
		target_compile_options(${target_name} PRIVATE /W4)   	#warning level 4
		target_compile_options(${target_name} PRIVATE /we6244)  #warning C6244: local declaration of <variable> hides previous declaration
		target_compile_options(${target_name} PRIVATE /we6246)  #warning C6246: Local declaration of <variable> hides declaration of same name in outer scope
		
		# disable warning
		target_compile_options(${target_name} PRIVATE /wd4100)	#warning C4100: unreferenced formal parameter in function
		target_compile_options(${target_name} PRIVATE /wd4127) 	#warning C4127: conditional expression is constant		
		target_compile_options(${target_name} PRIVATE /wd4200)	#warning C4200 : nonstandard extension used: zero-sized array in struct/union				
		target_compile_options(${target_name} PRIVATE /wd4201)	#warning C4201: nonstandard extension used: nameless struct/union
		target_compile_options(${target_name} PRIVATE /wd4275)	#warning C4275: non dll-interface class 'std::runtime_error' used as base for dll-interface class 'fmt::v10::format_error'
		target_compile_options(${target_name} PRIVATE /wd4702)  #warning C4702: unreachable code (seems vc has bug when handle if constexpr() )

	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GCC")
		target_compile_options(${target_name} PRIVATE	-Wall 
														-Werror
														#-Wextra 
														-Wpedantic 
														-Wno-c++98-compat-pedantic
														-Wno-exit-time-destructors # maybe ?
														-Wno-extra-semi-stmt
														-Wno-gnu-anonymous-struct
														-Wno-pre-c++17-compat
														-Wno-reserved-identifier
														-Wno-missing-prototypes
														-Wno-nested-anon-types
														-Wno-newline-eof
														-Wno-undef
														-Wno-unused-parameter
														-Wno-unused-local-typedef
														#
														-mavx #enable AVX
														)

	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		# message("ax_set_warning_level Clang")
		target_compile_options(${target_name} PRIVATE	-Wall 
														-Werror
														#-Wextra 
														-Wpedantic 
														-Wno-covered-switch-default
														-Wno-c++98-compat-pedantic
														-Wno-exit-time-destructors # maybe ?
														-Wno-extra-semi-stmt
														-Wno-gnu-anonymous-struct
														-Wno-gnu-zero-variadic-macro-arguments
														-Wno-language-extension-token  # DX12: IID_PPV_ARGS
														-Wno-missing-prototypes
														-Wno-newline-eof
														-Wno-nested-anon-types
														-Wno-missing-noreturn
														-Wno-pre-c++17-compat
														-Wno-reserved-identifier
														-Wno-switch-enum
														-Wno-undef
														-Wno-undefined-func-template
														-Wno-unreachable-code-break														
														-Wno-unreachable-code
														-Wno-unused-parameter														
														-Wno-unused-local-typedef
														-Wno-unused-member-function
														#
														-mavx #enable AVX
														)
	endif()
endfunction()

function(ax_source_group src_path src_files)
	foreach(FILE ${src_files}) 
		get_filename_component(PARENT_DIR "${FILE}" PATH)
		file(RELATIVE_PATH PARENT_DIR ${src_path} ${PARENT_DIR})
		string(REPLACE "/" "\\" GROUP "${PARENT_DIR}")
		set(GROUP "${GROUP}")
		source_group("${GROUP}" FILES "${FILE}")	
	endforeach()
endfunction()

function(ax_gen_folder_all_h_RECURSE src_path)
	set(text "/* !!! Generated by ax_common.cmake */ \n")
	string(APPEND text "#pragma once\n")
	string(APPEND text "\n")

	get_filename_component(src_path_basename ${src_path} NAME)

	file(GLOB file_list LIST_DIRECTORIES true "${src_path}/*")
	foreach(item ${file_list})
		file(RELATIVE_PATH rel_path ${src_path} ${item})
		get_filename_component(basename ${item} NAME)
		get_filename_component(ext ${item} EXT)

		if(IS_DIRECTORY ${item})
			if (EXISTS "${item}/_SKIP_INCLUDE_ALL_.h")
				STRING(APPEND text "// #include \"${rel_path}/_ALL_${basename}.h\" // SKIP \n")
			else()
				STRING(APPEND text "#include \"${rel_path}/_ALL_${basename}.h\"\n")
			endif()
			
			ax_gen_folder_all_h_RECURSE(${item})
		elseif(basename STREQUAL "_SKIP_INCLUDE_ALL_.h")
			# skip _SKIP_INCLUDE_ALL_.h
		elseif(basename STREQUAL "_ALL_${src_path_basename}.h")
			# skip itself
		elseif("${ext}" STREQUAL ".h")
			STRING(APPEND    text "#include \"${rel_path}\"\n")
		endif()
	endforeach()

	set(out_filename "${src_path}/_ALL_${src_path_basename}.h")
	# message("write generated file ${out_filename}")
	ax_write_to_file("${out_filename}" "${text}")

endfunction()

function(ax_gen_folder_all_h target_name src_path)
	ax_gen_folder_all_h_RECURSE("${src_path}/src/${target_name}")
endfunction()

function(ax_gen_test_all_cpp_RECURSE src_path func_name)
	set(text "/* !!! Generated by ax_common.cmake */ \n\n")
	string(APPEND text "#include \"AxUnitTest.h\"\n\n")

	get_filename_component(src_path_basename ${src_path} NAME)
	string(APPEND text "void ${func_name}() {\n")

	file(GLOB file_list LIST_DIRECTORIES true "${src_path}/*")
	foreach(item ${file_list})
		file(RELATIVE_PATH rel_path "${src_path}" "${item}")
		get_filename_component(basename "${item}" NAME)
		get_filename_component(basenameno_ext "${item}" NAME_WE)
		get_filename_component(ext "${item}" EXT)

		if(IS_DIRECTORY ${item})
			set(test_group_func_name "${func_name}_${basenameno_ext}")
			STRING(APPEND    text "    AX_TEST_GROUP(${test_group_func_name})\n")
			ax_gen_test_all_cpp_RECURSE("${item}" "${test_group_func_name}")
		elseif(basename STREQUAL "_ALL_${src_path_basename}.cpp")
			# skip itself
		elseif("${ext}" STREQUAL ".cpp")
			STRING(APPEND    text "    AX_TEST_FUNC(${basenameno_ext})\n")
		endif()
	endforeach()	
	string(APPEND text "}\n")

	set(out_filename "${src_path}/_ALL_${src_path_basename}.cpp")
	ax_write_to_file("${out_filename}" "${text}" )
endfunction()

function(ax_gen_test_all_cpp target_name src_path)
	ax_gen_test_all_cpp_RECURSE("${src_path}/src/${target_name}" "${target_name}")
endfunction()


function(ax_target_source_from_folder target_name src_path)
	get_target_property(target_type ${target_name} TYPE)

	file(GLOB_RECURSE all_files  "${src_path}/src/*.*")
	file(GLOB_RECURSE h_files    "${src_path}/src/*.h")
	file(GLOB_RECURSE cpp_files  "${src_path}/src/*.cpp")
	file(GLOB_RECURSE cppm_files "${src_path}/src/*.cppm")

	ax_source_group(${src_path} "${all_files}")

	set(other_files ${all_files})
	list(REMOVE_ITEM other_files ${cpp_files} ${h_files})

	target_sources(${target_name} PRIVATE ${h_files})
	target_sources(${target_name} PRIVATE ${cpp_files})
	target_sources(${target_name} PRIVATE ${other_files})

	if ("${target_type}" STREQUAL "INTERFACE_LIBRARY")
	else()
		target_sources(${target_name} PUBLIC 
			FILE_SET cxx_modules 
			TYPE CXX_MODULES 
			FILES ${cppm_files})
	endif()

	if(CMAKE_GENERATOR STREQUAL Xcode)
		set_source_files_properties(${cpp_files} PROPERTIES LANGUAGE OBJCXX)
	endif()

endfunction()

function(ax_target_set_header_only_common_properties target_name)
endfunction()

function(ax_target_set_common_properties target_name)
	ax_target_set_header_only_common_properties(${target_name})

	ax_set_warning_level(${target_name})
	set_target_properties(${target_name} PROPERTIES
							UNITY_BUILD ON
							UNITY_BUILD_MODE BATCH
							UNITY_BUILD_BATCH_SIZE 8)

#	target_precompile_headers(${target_name} PRIVATE src/${target_name}-pch.h)
	target_compile_definitions(${target_name} PUBLIC -DAX_BUILD_${target_name})
	target_compile_definitions(${target_name} PUBLIC 
		$<$<CONFIG:Debug>:AX_BUILD_CONFIG_Debug>
		$<$<CONFIG:MinSizeRel>:AX_BUILD_CONFIG_MinSizeRel>	
		$<$<CONFIG:Release>:AX_BUILD_CONFIG_Release>
		$<$<CONFIG:RelWithDebInfo>:AX_BUILD_CONFIG_RelWithDebInfo>
	)
	target_compile_definitions(${target_name} PUBLIC -DUNICODE -D_UNICODE)
endfunction()

#------- add build target ----------------------------------

function(ax_add_header_only_library target_name src_path)
	add_library(${target_name} INTERFACE)
	ax_target_source_from_folder(${target_name} ${src_path})

	target_include_directories(${target_name} INTERFACE src)
	ax_target_set_header_only_common_properties(${target_name})
endfunction()


function(ax_add_library target_name src_path)
	add_library(${target_name})
	ax_target_source_from_folder(${target_name} ${src_path})

	target_include_directories(${target_name} PUBLIC src)
	ax_target_set_common_properties(${target_name})
endfunction()

function(ax_add_executable target_name src_path)
	add_executable(${target_name})
	ax_target_source_from_folder(${target_name} ${src_path})

	target_include_directories(${target_name} PRIVATE src)
	ax_target_set_common_properties(${target_name})
endfunction()

function(ax_add_gui_executable target_name src_path)
	add_executable(${target_name} WIN32)
	ax_target_source_from_folder(${target_name} ${src_path})

	set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/src/AppIcon.rc")
	set_target_properties(${target_name} PROPERTIES COMPILE_FLAGS "-D_QWE_GUI_EXECUTABLE")

	target_include_directories(${target_name} PRIVATE src)
	ax_target_set_common_properties(${target_name})
endfunction()
