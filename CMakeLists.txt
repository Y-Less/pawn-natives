cmake_minimum_required(VERSION 3.10)

if (NOT MAIN_RUN)
		project(dependencies)

		include(ExternalProject)

		# if (WIN32)
				# ExternalProject_Add (
						# cmake-modules
						# PREFIX "cmake-modules"
						# GIT_REPOSITORY "https://github.com/Zeex/cmake-modules.git"
						# GIT_TAG "master"
						# UPDATE_COMMAND ""
						# CONFIGURE_COMMAND mkdir ..\\..\\..\\cmake
						# BUILD_COMMAND cp ${CMAKE_SOURCE_DIR}/cmake-modules/src/cmake-modules/*.cmake ${CMAKE_SOURCE_DIR}/cmake )
		# else ()
				ExternalProject_Add (
						cmake-modules
						PREFIX "cmake-modules"
						GIT_REPOSITORY "https://github.com/Zeex/cmake-modules.git"
						GIT_TAG "master"
						UPDATE_COMMAND ""
						CONFIGURE_COMMAND "" #mkdir ../../../cmake
						BUILD_COMMAND "" #cp ${CMAKE_SOURCE_DIR}/cmake-modules/src/cmake-modules/*.cmake ${CMAKE_SOURCE_DIR}/cmake )
						INSTALL_COMMAND "" )
		# endif()
		list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules/src/cmake-modules)

		ExternalProject_Add (
				samp-plugin-sdk
				PREFIX "samp-plugin-sdk"
				GIT_REPOSITORY "https://github.com/Y-Less/samp-plugin-sdk.git"
				GIT_TAG "master"
				UPDATE_COMMAND ""
				CONFIGURE_COMMAND ""
				BUILD_COMMAND ""
				INSTALL_COMMAND "" )

		ExternalProject_Add (
				subhook
				PREFIX "subhook"
				GIT_REPOSITORY "https://github.com/Zeex/subhook.git"
				GIT_TAG "master" )

		ExternalProject_Add (
				sampgdk
				PREFIX "sampgdk"
				GIT_REPOSITORY "https://github.com/Zeex/sampgdk.git"
				GIT_TAG "master"
				UPDATE_COMMAND ""
				BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release
				INSTALL_COMMAND ${CMAKE_COMMAND} --build . --config Release --target Install
				CMAKE_ARGS "-DSAMP_SDK_ROOT=${CMAKE_SOURCE_DIR}/samp-plugin-sdk/src/samp-plugin-sdk" )

		add_custom_command(TARGET sampgdk POST_BUILD
				COMMAND "${CMAKE_COMMAND}" ${CMAKE_SOURCE_DIR} -DMAIN_RUN=TRUE
				COMMAND "${CMAKE_COMMAND}" --build ${CMAKE_SOURCE_DIR} )
else ()
		project(pawn-natives)

		#if (${SAMP_CMAKE_MODULES} STREQUAL "NOTFOUND")
		#		message("------------------------------------------------")
		#		message("| Initial clone complete, please re-run cmake. |")
		#		message("------------------------------------------------")
		#endif ()
		# Now that we have them, use them.
		list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules/src/cmake-modules)
		include(AMXConfig) #OPTIONAL RESULT_VARIABLE SAMP_CMAKE_MODULES)
		include(AddSAMPPlugin) #OPTIONAL)

		include_directories(
				samp-plugin-sdk/src/samp-plugin-sdk/
				subhook/src/
				sampgdk/src/sampgdk/include/
				sampgdk/src/sampgdk-build/include/
		)

		#find_package(sampgdk REQUIRED)
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)
		add_definitions(-DSUBHOOK_IMPLEMENTATION)
		add_definitions(-DHAVE_STDINT_H=1)

		add_samp_plugin(pawn-natives
				test.cpp
				subhook/src/subhook/subhook.c
				samp-plugin-sdk/src/samp-plugin-sdk/amxplugin.cpp
				samp-plugin-sdk/src/samp-plugin-sdk/amxplugin2.cpp
		)

		if (WIN32)
				target_link_libraries(pawn-natives
						${CMAKE_SOURCE_DIR}/sampgdk/src/sampgdk-build/Release/sampgdk4.lib
				)
		else ()
		endif ()
endif ()

