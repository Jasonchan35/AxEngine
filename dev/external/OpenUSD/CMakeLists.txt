set(PROJECT_NAME OpenUSD-prebuilt)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

add_library(${PROJECT_NAME} IMPORTED SHARED GLOBAL)

set_target_properties(${PROJECT_NAME} PROPERTIES 
	INTERFACE_LINK_DIRECTORIES    "${CMAKE_CURRENT_SOURCE_DIR}/_build/lib"
	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/_build/include")

# target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/_build/include")
# target_link_directories(   ${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/_build/lib")



file(GLOB_RECURSE all_lib "${CMAKE_CURRENT_SOURCE_DIR}/_build/lib/*.lib")
foreach(filepath IN LISTS all_lib)
#	message("import lib ${filepath}")
	set_target_properties(${PROJECT_NAME} PROPERTIES IMPORTED_IMPLIB ${filepath})
	get_filename_component(filename ${filepath} NAME)
#	message("lib ${filename}")
	target_link_libraries(${PROJECT_NAME} INTERFACE "${filename}")
endforeach()

file(GLOB_RECURSE all_dll 
	"${CMAKE_CURRENT_SOURCE_DIR}/_build/bin/*.dll" 
	"${CMAKE_CURRENT_SOURCE_DIR}/_build/lib/*.dll" 
	"${CMAKE_CURRENT_SOURCE_DIR}/_build/src/tbb/bin/intel64/vc14/*.dll")
foreach(src_filename IN LISTS all_dll)
#	message("import dll ${src_filename}")
	set_target_properties(${PROJECT_NAME} PROPERTIES IMPORTED_LOCATION ${src_filename})
	get_filename_component(filename ${src_filename} NAME)

	if(AX_IS_MULTI_CONFIG)
		foreach(C IN LISTS CMAKE_CONFIGURATION_TYPES)
			ax_copy_file_if_newer(${src_filename} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${C}/${filename}")
		endforeach()
	else()
		ax_copy_file_if_newer(${src_filename} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${filename}")
	endif()
endforeach()

if(AX_IS_MULTI_CONFIG)
	foreach(CONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
		ax_copy_dir_if_newer("${CMAKE_CURRENT_SOURCE_DIR}/_build/lib/usd"     "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CONFIG}/usd")
	endforeach()
	ax_copy_dir_if_newer("${CMAKE_CURRENT_SOURCE_DIR}/_build/plugin"  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugin")
else()
	ax_copy_dir_if_newer("${CMAKE_CURRENT_SOURCE_DIR}/_build/plugin/usd"  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugin/usd")
#	ax_copy_dir_if_newer("${CMAKE_CURRENT_SOURCE_DIR}/_build/lib/usd"     "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/usd")
endif()



