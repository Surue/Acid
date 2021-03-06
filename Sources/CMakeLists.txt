add_library(Acid)
# This file is quite big, so for readability we include instead...
# This uses target_sources(Acid) to define its headers & source files
include(CMakeSources.cmake)

# Manually defined by us
if(NOT glfw_FOUND)
	add_dependencies(Acid glfw)
endif()
# Defined in find_package(PhysFS)
if(NOT PHYSFS_FOUND)
	if(PHYSFS_BUILD_STATIC)
		add_dependencies(Acid physfs-static)
	else()
		add_dependencies(Acid physfs)
	endif()
endif()
# Defined in find_package(Freetype)
if(NOT Freetype_FOUND)
	add_dependencies(Acid freetype)
endif()
# Manually defined by us
if(NOT SPIRV_FOUND)
	add_dependencies(Acid SPIRV)
endif()
# Defined in find_package(Bullet)
if(NOT BULLET_FOUND)
	add_dependencies(Acid BulletDynamics)
endif()

target_compile_features(Acid PUBLIC cxx_std_17)
set_target_properties(Acid PROPERTIES
		POSITION_INDEPENDENT_CODE ON
		FOLDER "Acid"
		)

if(BUILD_SHARED_LIBS)
	set_target_properties(Acid PROPERTIES DEFINE_SYMBOL "ACID_EXPORTS")

	if(WIN32)
		set_target_properties(Acid PROPERTIES PREFIX "")
		set_target_properties(Acid PROPERTIES DEBUG_POSTFIX "")
	elseif(UNIX AND APPLE)
		set_target_properties(Acid PROPERTIES INSTALL_NAME_DIR "lib${LIB_SUFFIX}")
	endif()
	target_compile_options(Acid
			PRIVATE
			$<$<CXX_COMPILER_ID:GNU>:-fvisibility=hidden>
			$<$<CXX_COMPILER_ID:AppleClang>:-fno-common>
			)
else()
	target_compile_definitions(Acid PUBLIC "ACID_STATICLIB")
endif()

target_compile_definitions(Acid
		PUBLIC
		# If the CONFIG is Debug or RelWithDebInfo, define ACID_VERBOSE
		# Works on both single and mutli configuration
		ACID_VERBOSE # $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:ACID_VERBOSE>
		# 32-bit
		$<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:ACID_BUILD_32BIT>
		# 64-bit
		$<$<EQUAL:8,${CMAKE_SIZEOF_VOID_P}>:ACID_BUILD_64BIT>
		# Windows
		$<$<PLATFORM_ID:Windows>:ACID_BUILD_WINDOWS WIN32_LEAN_AND_MEAN NOMINMAX>
		# Linux
		$<$<PLATFORM_ID:Linux>:ACID_BUILD_LINUX>
		# macOS
		$<$<PLATFORM_ID:Darwin>:ACID_BUILD_MACOS>
		# MSVC
		$<$<CXX_COMPILER_ID:MSVC>:ACID_BUILD_MSVC _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS>
		# Clang/AppleClang
		$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:ACID_BUILD_CLANG>
		# GNU/GCC
		$<$<CXX_COMPILER_ID:GNU>:ACID_BUILD_GNU __USE_MINGW_ANSI_STDIO=0>
		)
target_compile_options(Acid
		PUBLIC
		# Disables symbol warnings.
		$<$<CXX_COMPILER_ID:MSVC>:/wd4251 /wd4592>
		PRIVATE
		# Enables SSE4.1, it is also possible to use SSE2 with -msse2
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-msse4.1>
		# Enabled SSE2 for MSVC for 32-bit.
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>>:/arch:SSE2>
		)

target_include_directories(Acid
		PUBLIC
		# Helps the includes find what they need at build-time
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		# Helps the includes find what they need at runtime
		# Although this also allows people to not prefix "Acid" before includes as well
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
		PRIVATE
		# Since building locally from the submodules won't always create these vars.
		# We have to do a simple check if they exist or they will cause errors
		$<$<BOOL:${BULLET_INCLUDE_DIRS}>:${BULLET_INCLUDE_DIRS}>
		$<$<BOOL:${PHYSFS_INCLUDE_DIR}>:${PHYSFS_INCLUDE_DIR}>
		$<$<BOOL:${SPIRV_INCLUDE_DIR}>:${SPIRV_INCLUDE_DIR}>
		)

target_link_libraries(Acid
		PUBLIC
		# All IMPORTED targets, which automatically handles includes
		Threads::Threads
		Vulkan::Vulkan
		# Unix
		${CMAKE_DL_LIBS}
		# Windows
		$<$<PLATFORM_ID:Windows>:ws2_32>
		$<$<PLATFORM_ID:Windows>:dbghelp>
		PRIVATE
		# More IMPORTED
		glfw
		OpenAL::OpenAL
		# Not IMPORTED targets, requires manual includes handling
		# When built locally from submodules, these are merely the names..
		# of the library targets that exist in-scope, else they're set..
		# from find_package() calls
		$<$<BOOL:${Freetype_FOUND}>:Freetype::Freetype>
		$<$<NOT:$<BOOL:${Freetype_FOUND}>>:${FREETYPE_LIBRARIES}>
		${PHYSFS_LIBRARY}
		${BULLET_LIBRARIES}
		${SPIRV_LIBRARY}
		)

# Creates a symbolic link from Resources to Resources/Engine in the binary folder
if(ACID_LINK_RESOURCES)
	get_filename_component(CURRENT_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} PATH)
	add_custom_command(TARGET Acid POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/Resources
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${CURRENT_PARENT_DIR}/Resources ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/Resources/Engine
			)
endif()

# Installs all headers, preserving their file structure
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
		FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hpp"
		)
# If strictly using system libs, we can generate an export & install it
if(_ACID_ALL_SYSTEM_LIBS)
	install(TARGETS Acid
			# Auto-generates an export to install
			EXPORT
			AcidTargets
			LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			)
	# Install the auto-generated export
	install(EXPORT AcidTargets
			# Makes the end-user imported library Acid::Acid instead of just Acid
			NAMESPACE Acid::
			DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
			)
	# Defines write_basic_package_version_file
	include(CMakePackageConfigHelpers)
	# Dynamically writes the path to the resources dir (if installed) into the AcidConfig.cmake file
	configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/AcidConfig.cmake.in"
			"${PROJECT_BINARY_DIR}/AcidConfig.cmake"
			INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
			PATH_VARS CMAKE_INSTALL_DATADIR
			NO_CHECK_REQUIRED_COMPONENTS_MACRO
			)
	# Auto-generates a version file based on the current version specified in project()
	write_basic_package_version_file(
			"${PROJECT_BINARY_DIR}/AcidConfigVersion.cmake"
			COMPATIBILITY SameMajorVersion
	)
	# Install the auto-generated version file and AcidConfig
	install(
			FILES
			"${PROJECT_BINARY_DIR}/AcidConfig.cmake"
			"${PROJECT_BINARY_DIR}/AcidConfigVersion.cmake"
			DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
	)
else()
	# Install without an export since we're using 1 (or more) non-system libs
	install(TARGETS Acid
			LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
			)
endif()
