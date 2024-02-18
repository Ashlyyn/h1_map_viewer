cmake_minimum_required(VERSION 3.19)
project(h1_map_viewer)
set(CMAKE_CXX_STANDARD 23)

include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

find_package(freetype REQUIRED)
find_package(glew REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(h1_map_viewer 
	src/cg_cgame.cpp src/com.cpp src/font.cpp src/fs_files.cpp src/gfx.cpp 
	src/gfx_backend.cpp  src/input.cpp src/main.cpp src/sys.cpp
 "src/db_files.cpp" "src/cmd_commands.cpp" "src/cl.cpp" "src/devgui.cpp" "src/gfx_text.cpp" "src/gfx_shaders.cpp" "src/devcon.cpp" "src/dvar.cpp" "src/con_console.cpp")

target_link_libraries(h1_map_viewer PRIVATE Freetype::Freetype)
target_link_libraries(h1_map_viewer PRIVATE GLEW::GLEW)
# GLM is header-only, nothing to link

# SDL3 and SDL3_image aren't available from vcpkg. SDL_image contains SDL as a
# dependency, so it can be used instead of downloading another copy.
#
# Both have to be built out-of-tree.
add_subdirectory(deps/SDL_image/external/SDL deps/SDL_image/external/SDL_build)
add_subdirectory(deps/SDL_image deps/SDL_image_build)

# Since they aren't handled by vcpkg, the library locations have to be 
# specified manually.
add_library(sdl_image SHARED IMPORTED GLOBAL)
set_property(TARGET sdl_image PROPERTY IMPORTED_IMPLIB_DEBUG 
	deps/SDL_image_build/Debug/SDL3_image
)
if(WIN32)
	set_property(TARGET sdl_image PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image_build/Debug/SDL3_image.dll
	)
elseif(APPLE)
set_property(TARGET sdl_image PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image_build/Debug/SDL3_image.dylib
	)
elseif(UNIX)
	set_property(TARGET sdl_image PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image_build/Debug/SDL3_image.so
	)
else()
	message("what the hell platform are you building this for?")
	set_property(TARGET sdl_image PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image_build/Debug/SDL3_image
	)
endif()
set_property(TARGET sdl_image PROPERTY IMPORTED_IMPLIB_RELEASE 
	deps/SDL_image_build/Debug/SDL3_image
)
if(WIN32)
	set_property(TARGET sdl_image PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image_build/Debug/SDL3_image.dll
	)
elseif(APPLE)
set_property(TARGET sdl_image PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image_build/Debug/SDL3_image.dylib
	)
elseif(UNIX)
	set_property(TARGET sdl_image PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image_build/Debug/SDL3_image.so
	)
else()
	message("what the hell platform are you building this for?")
	set_property(TARGET sdl_image PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image_build/Debug/SDL3_image
	)
endif()

add_library(sdl SHARED IMPORTED GLOBAL)
set_property(TARGET sdl PROPERTY IMPORTED_IMPLIB_DEBUG 
	deps/SDL_image/external/SDL_build/Debug/SDL3
)
if(WIN32)
	set_property(TARGET sdl PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image/external/SDL_build/Debug/SDL3.dll
	)
elseif(APPLE)
set_property(TARGET sdl PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image/external/SDL_build/Debug/SDL3.dylib
	)
elseif(UNIX)
	set_property(TARGET sdl PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image/external/SDL_build/Debug/SDL3.so
	)
else()
	message("what the hell platform are you building this for?")
	set_property(TARGET sdl PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image/external/SDL_build/Debug/SDL3
	)
endif()
set_property(TARGET sdl PROPERTY IMPORTED_IMPLIB_RELEASE 
	deps/SDL_image/external/SDL_build/Debug/SDL3
)
if(WIN32)
	set_property(TARGET sdl PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image/external/SDL_build/Debug/SDL3.dll
	)
elseif(APPLE)
	set_property(TARGET sdl PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image/external/SDL_build/Debug/SDL3.dylib
	)
elseif(UNIX)
	set_property(TARGET sdl PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image/external/SDL_build/Debug/SDL3.so
	)
else()
	message("what the hell platform are you building this for?")
	set_property(TARGET sdl PROPERTY IMPORTED_LOCATION_DEBUG 
		deps/SDL_image/external/SDL_build/Debug/SDL3
	)
endif()

target_link_libraries(h1_map_viewer PRIVATE 
	deps/SDL_image/external/SDL_build/Debug/SDL3
)
target_link_libraries(h1_map_viewer PRIVATE 
	deps/SDL_image_build/Debug/SDL3_image
)

target_include_directories(h1_map_viewer PRIVATE glm::glm)
target_include_directories(h1_map_viewer PRIVATE deps/SDL_image/include)
target_include_directories(h1_map_viewer PRIVATE 
	deps/SDL_image/external/SDL/include
)

set_target_properties(h1_map_viewer PROPERTIES WORKING_DIRECTORY ./build)

# Copy SDL3 and SDL3_image's dynamic libraries to the executable dir
# (the libraries from vcpkg do this manually, so it only needs to be done here
# for SDL3 and SDL3_image).
add_custom_command (TARGET h1_map_viewer POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:sdl> $<TARGET_FILE_DIR:h1_map_viewer>
)

add_custom_command (TARGET h1_map_viewer POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:sdl_image> $<TARGET_FILE_DIR:h1_map_viewer>
)