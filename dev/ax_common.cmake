include_guard(GLOBAL)

message("CMAKE_SYSTEM_NAME         = ${CMAKE_SYSTEM_NAME}")
message("CMAKE_GENERATOR_PLATFORM  = ${CMAKE_GENERATOR_PLATFORM}")
message("CMAKE_GENERATOR_TOOLSET   = ${CMAKE_GENERATOR_TOOLSET}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#---- helper functions ----------

# function(ax_test_call_by_reference varName)
#   SET(${varName} "value to return" PARENT_SCOPE)
# endfunction()

set(AX_CppHeaerTool "${CMAKE_CURRENT_BINARY_DIR}/bin/Debug/AxCppHeaderTool")

function(ax_cpp_header_tool target path)
	add_custom_target(${target}_CppHeaderTool
		COMMAND ${CMAKE_COMMAND} -E echo "${target} Running AxCppHeaderTool..."
		COMMAND ${AX_CppHeaerTool} -moduleName=${target} -outPath=${path} ${path}
		WORKING_DIRECTORY ${path}
	)
	add_dependencies(${target}_CppHeaderTool "AxCppHeaderTool")
	add_dependencies(${target} ${target}_CppHeaderTool)
endfunction()

function(ax_write_to_file filename text)
	message("ax_write_to_file(\"${filename}\")")
	file(GENERATE OUTPUT ${filename} CONTENT "${text}" NEWLINE_STYLE UNIX)	
endfunction()

function(ax_copy_file src_filename dst_filename)
	message("ax_copy_file(\"${src_filename}\" \"${dst_filename}\")")
	file(COPY "${src_filename}" DESTINATION "${dst_filename}")
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
		# target_compile_options(${target_name} PRIVATE /Bt)	 	# show Build Timing

		target_compile_options(${target_name} PRIVATE /utf-8) 	# execution_character_set - otherwise std::format wouldn't check format in compile time for "char"
		target_compile_options(${target_name} PRIVATE /WX)    	#warning treated as error
		target_compile_options(${target_name} PRIVATE /W4)   	#warning level 4
#		target_compile_options(${target_name} PRIVATE /Wall)   	#warning level all
		target_compile_options(${target_name} PRIVATE /we6244)  #warning C6244: local declaration of <variable> hides previous declaration
		target_compile_options(${target_name} PRIVATE /we6246)  #warning C6246: Local declaration of <variable> hides declaration of same name in outer scope
#		target_compile_options(${target_name} PRIVATE /Zp16)    # struct alignment

		# re-enable warning disabled by default to level 4
		# VS2017 or later
		target_compile_options(${target_name} PRIVATE /w45038)  # data member 'member1' will be initialized after data member 'member2'
		target_compile_options(${target_name} PRIVATE /w45039)  # 'function': pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
		target_compile_options(${target_name} PRIVATE /w45041)  # 'member-name': out-of-line definition for constexpr static data member is not needed and is deprecated in C++17
		target_compile_options(${target_name} PRIVATE /w45042)  # 'function': function declarations at block scope cannot be specified 'inline' in standard C++; remove 'inline' specifier 
#		target_compile_options(${target_name} PRIVATE /w45045)  # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

		# VS2019 or later
		target_compile_options(${target_name} PRIVATE /w45052)  # Keyword 'keyword-name' was introduced in C++ version and requires use of the 'option' command-line option
		target_compile_options(${target_name} PRIVATE /w45204)  # A class with virtual functions has non-virtual trivial destructor
		target_compile_options(${target_name} PRIVATE /w45214)  # applying 'keyword' to an operand with a volatile qualified type is deprecated in C++20
		target_compile_options(${target_name} PRIVATE /w45215)  # 'function-parameter' a function parameter with a volatile qualified type is deprecated in C++20
		target_compile_options(${target_name} PRIVATE /w45216)  # 'return-type' a volatile qualified return type is deprecated in C++20
		target_compile_options(${target_name} PRIVATE /w45217)  # a structured binding declaration that includes volatile is deprecated in C++20
		target_compile_options(${target_name} PRIVATE /w45219)  # implicit conversion from 'type-1' to 'type-2', possible loss of data
		target_compile_options(${target_name} PRIVATE /w45220)  # 'member': a non-static data member with a volatile qualified type no longer implies that compiler generated copy/move constructors and copy/move assignment operators are not trivial
		target_compile_options(${target_name} PRIVATE /w45233)  # explicit lambda capture 'identifier' is not used
		target_compile_options(${target_name} PRIVATE /w45240)  # attribute-name': attribute is ignored in this syntactic position  
		target_compile_options(${target_name} PRIVATE /w45243)  # 'type-name': using incomplete class 'class-name' can cause potential one definition rule violation due to ABI limitation
		target_compile_options(${target_name} PRIVATE /w45245)  # 'function': unreferenced function with internal linkage has been removed
		target_compile_options(${target_name} PRIVATE /w45246)  # 'member': the initialization of a subobject should be wrapped in braces
		target_compile_options(${target_name} PRIVATE /w45247)  # Section 'section-name' is reserved for C++ dynamic initialization. Manually creating the section will interfere with C++ dynamic initialization and may lead to undefined behavior
		target_compile_options(${target_name} PRIVATE /w45248)  # Section 'section-name' is reserved for C++ dynamic initialization. Variable manually put into the section may be optimized out and its order relative to compiler generated dynamic initializers is unspecified
		
		# VS2022 or later
		target_compile_options(${target_name} PRIVATE /w45249)  # 'bitfield' of type 'enumeration_name' has named enumerators with values that cannot be represented in the given bit field width of 'bitfield_width'
		target_compile_options(${target_name} PRIVATE /w45250)  # 'function_name': intrinsic function not declared.
		target_compile_options(${target_name} PRIVATE /w45251)  # segment-name changed after including header 
		target_compile_options(${target_name} PRIVATE /w45254)  # language feature 'terse static assert' requires compiler flag '/std:c++17
		target_compile_options(${target_name} PRIVATE /w45256)  # 'enumeration': a non-defining declaration of an enumeration with a fixed underlying type is only permitted as a standalone declaration
		target_compile_options(${target_name} PRIVATE /w45258)  # explicit capture of 'symbol' is not required for this use
		target_compile_options(${target_name} PRIVATE /w45259)  # 'specialized-type': explicit specialization requires 'template <>'
		target_compile_options(${target_name} PRIVATE /w45262)  # implicit fall-through occurs here; are you missing a break statement? Use [[fallthrough]] when a break statement is intentionally omitted between cases
		target_compile_options(${target_name} PRIVATE /w45263)  # calling 'std::move' on a temporary object prevents copy elision
		target_compile_options(${target_name} PRIVATE /w45264)  # 'variable-name': 'const' variable is not used
		target_compile_options(${target_name} PRIVATE /w45266)  # 'const' qualifier on return type has no effect
#		target_compile_options(${target_name} PRIVATE /w45267)  # definition of implicit copy constructor/assignment operator for 'type' is deprecated because it has a user-provided assignment operator/copy constructor

		# disable warning
		target_compile_options(${target_name} PRIVATE /wd4100)	#warning C4100: unreferenced formal parameter in function
		target_compile_options(${target_name} PRIVATE /wd4127) 	#warning C4127: conditional expression is constant		
		target_compile_options(${target_name} PRIVATE /wd4201)	#warning C4201: nonstandard extension used: nameless struct/union
		target_compile_options(${target_name} PRIVATE /wd4275)	#warning C4275: non dll-interface class 'std::runtime_error' used as base for dll-interface class 'fmt::v10::format_error'
		target_compile_options(${target_name} PRIVATE /wd4702)  #warning C4702: unreachable code (seems vc has bug when handle if constexpr() )
		target_compile_options(${target_name} PRIVATE /wd4714)  #Warning C4714 : function marked as __forceinline not inlined

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
														-Wno-invalid-offsetof # IArrayStorage - offsetof(SmallStorage_Dummy, _data)
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
														-Wno-switch
														-Wno-undef
														-Wno-undefined-func-template
														-Wno-unreachable-code-break
														-Wno-unreachable-code
														-Wno-unused-parameter
														-Wno-unused-local-typedef
														-Wno-unused-member-function
														-Wno-unused-const-variable
														-Wno-uninitialized
														#
														-mavx #enable AVX
														-mfma #enable '_mm_fmadd_ps' requires target feature 'fma'
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

function(ax_gen_test_all_cpp_RECURSE output_text src_path func_name)
	get_filename_component(src_path_basename ${src_path} NAME)
	set(text "${${output_text}}")
	set(text2 "")

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
			ax_gen_test_all_cpp_RECURSE(text2 "${item}" "${test_group_func_name}")
		elseif(basename STREQUAL "_ALL_${src_path_basename}.cpp")
			# skip itself
		elseif("${ext}" STREQUAL ".cpp")
			STRING(APPEND    text "    AX_TEST_FUNC(${basenameno_ext})\n")
		endif()
	endforeach()	
	string(APPEND text "}\n\n")

	# string(APPEND ${output_text} "${text}" PARENT_SCOPE)
	set(${output_text} "${text2}${text}" PARENT_SCOPE)

endfunction()

function(ax_gen_test_all_cpp target_name src_path)
	set(text2 "")	
	ax_gen_test_all_cpp_RECURSE(text2 "${src_path}/src/${target_name}" "${target_name}")

	set(output_text "/* !!! Generated by ax_common.cmake - ax_gen_test_all_cpp */ \n\n")
	string(APPEND output_text "#include \"AxUnitTest.h\"\n")
	string(APPEND output_text "import AxCore.UnitTest;\n\n")
	string(APPEND output_text "${text2}")
	
	set(out_filename "${src_path}/src/_ALL_Test_.cpp")
	ax_write_to_file("${out_filename}" "${output_text}" )
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

	target_compile_definitions(${target_name} PUBLIC -DAX_BUILD_${target_name})
	target_compile_definitions(${target_name} PUBLIC 
		$<$<CONFIG:Debug>:AX_BUILD_CONFIG_Debug>
		$<$<CONFIG:MinSizeRel>:AX_BUILD_CONFIG_MinSizeRel>	
		$<$<CONFIG:Release>:AX_BUILD_CONFIG_Release>
		$<$<CONFIG:RelWithDebInfo>:AX_BUILD_CONFIG_RelWithDebInfo>
	)

	target_precompile_headers(${target_name} PRIVATE src/${target_name}-pch.h)

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
