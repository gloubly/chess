cmake_minimum_required (VERSION 3.8)

project ("Chess")

set (CMAKE_CXX_STANDARD 20)

include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)
#set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
#set(BUILD_GTEST ON CACHE BOOL "" FORCE)



enable_testing()
find_package(OpenGL REQUIRED)

include_directories(
	src
	include
	resources
	external
	external/glm
	external/glad/include
	external/stb
	external/freetype
)

set(EXT_SOURCES
	external/glad/src/glad.c
	external/stb/stb_image.c
)

# -------- #
#   main   #
# -------- #

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} 
	${SOURCES}
	${EXT_SOURCES}
)

target_link_directories(${PROJECT_NAME} PRIVATE 
	${CMAKE_SOURCE_DIR}/external/GLFW
	${CMAKE_SOURCE_DIR}/external/glm
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    glfw3
    OpenGL::GL
	glm_static
	${CMAKE_SOURCE_DIR}/external/freetype/freetype.lib
)



add_custom_target(copy-runtime-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/resources
)
add_dependencies(${PROJECT_NAME} copy-runtime-files)

# -------- #
#   test   #
# -------- #

if(BUILD_TESTING)
	file(GLOB PIECES_CPP "src/pieces/*.cpp")

	add_executable(unit_tests
		${PIECES_CPP}
		${EXT_SOURCES}
		src/piece.cpp
		src/board.cpp
		test/unit/board_unit.cpp
	)

	target_include_directories(unit_tests PRIVATE
		test
	)

	target_link_libraries(unit_tests
		PRIVATE
		gtest_main
	)

	include(GoogleTest)
	gtest_discover_tests(unit_tests)
endif()