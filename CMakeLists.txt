cmake_minimum_required(VERSION 3.19)
project(h1_map_viewer)
set(CMAKE_CXX_STANDARD 23)

include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

find_package(freetype REQUIRED)
find_package(glew REQUIRED)
find_package(glm CONFIG REQUIRED)

add_subdirectory(deps/SDL_image/external/SDL deps/SDL_image/external/SDL_build)
add_subdirectory(deps/SDL_image deps/SDL_image_build)

add_executable(h1_map_viewer 
	cg_cgame.cpp com.cpp font.cpp fs_files.cpp gfx.cpp gfx_backend.cpp 
	gfx_uniform.cpp input.cpp main.cpp sys.cpp
)

target_link_libraries(h1_map_viewer PRIVATE Freetype::Freetype)
target_link_libraries(h1_map_viewer PRIVATE GLEW::GLEW)

# CMake searches for SDL3.obj instead of SDL3.lib for some reason on Windows
# unless the extension is specified here.
if(MSVC)
	target_link_libraries(h1_map_viewer PRIVATE 
		${CMAKE_SOURCE_DIR}/deps/SDL_image/external/SDL_build/Debug/SDL3.lib
	)
	target_link_libraries(h1_map_viewer PRIVATE 
		${CMAKE_SOURCE_DIR}/deps/SDL_image_build/Debug/SDL3_image.lib
	)
else()
	target_link_libraries(h1_map_viewer PRIVATE 
		${CMAKE_SOURCE_DIR}/deps/SDL_image/external/SDL_build/Debug/SDL3.a
	)
	target_link_libraries(h1_map_viewer PRIVATE 
		${CMAKE_SOURCE_DIR}/deps/SDL_image_build/Debug/SDL3_image.a
	)
endif()

target_include_directories(h1_map_viewer PRIVATE glm::glm)
target_include_directories(h1_map_viewer PRIVATE deps/SDL_image/include)
target_include_directories(h1_map_viewer PRIVATE deps/SDL_image/external/SDL/include)