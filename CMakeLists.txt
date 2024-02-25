cmake_minimum_required(VERSION 3.19)
project(h1_map_viewer)
set(CMAKE_CXX_STANDARD 23)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(GCC true)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CLANG true)
endif()

set(CMAKE_BUILD_TYPE Release)

include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

find_package(Freetype REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(h1_map_viewer 
	src/cg_cgame.cpp src/com.cpp src/font.cpp src/fs_files.cpp src/gfx.cpp 
	src/gfx_backend.cpp  src/in_input.cpp src/main.cpp src/sys.cpp
 	src/db_files.cpp src/cmd_commands.cpp src/cl_client.cpp src/devgui.cpp 
	src/gfx_text.cpp src/gfx_shaders.cpp src/devcon.cpp src/dvar.cpp 
	src/con_console.cpp src/pm_pmove.cpp src/m_math.cpp
)


# GLM is header-only, nothing to link

# SDL3 and SDL3_image aren't available from vcpkg. SDL_image contains SDL as a
# dependency, so it can be used instead of downloading another copy.
#
# Both have to be built out-of-tree.
add_subdirectory(
	${CMAKE_SOURCE_DIR}/deps/SDL_image/external/SDL 
	${CMAKE_SOURCE_DIR}/deps/SDL_image/external/SDL_build
)
add_subdirectory(
	${CMAKE_SOURCE_DIR}/deps/SDL_image 
	${CMAKE_SOURCE_DIR}/deps/SDL_image_build
)

target_link_libraries(h1_map_viewer PRIVATE SDL3::SDL3)
target_link_libraries(h1_map_viewer PRIVATE SDL3_image::SDL3_image)
target_link_libraries(h1_map_viewer PRIVATE Freetype::Freetype)
target_link_libraries(h1_map_viewer PRIVATE GLEW::GLEW)
target_link_libraries(h1_map_viewer PRIVATE ZLIB::ZLIB)

target_include_directories(h1_map_viewer PRIVATE SDL3::SDL3)
target_include_directories(h1_map_viewer PRIVATE SDL3_image::SDL3_image)
target_include_directories(h1_map_viewer PRIVATE Freetype::Freetype)
target_include_directories(h1_map_viewer PRIVATE GLEW::GLEW)
target_include_directories(h1_map_viewer PRIVATE glm::glm)
target_include_directories(
	h1_map_viewer PRIVATE ${CMAKE_SOURCE_DIR}/deps/invader/include
)

set_target_properties(
	h1_map_viewer PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build
)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(h1_map_viewer PRIVATE _DEBUG=1)
else()
	target_compile_definitions(h1_map_viewer PRIVATE _DEBUG=0)
endif()

if(MSVC)
	target_compile_options(h1_map_viewer PRIVATE 
		"/permissive-" "/Zc:preprocessor" "/Zc:__cplusplus" 
		"/Zc:__STDC__" "/W4" "/wd4005" "/bigobj"
		"/WX"
	)
elseif(CLANG OR GCC)
	target_compile_options(h1_map_viewer PRIVATE "-pedantic-errors")
endif()