cmake_minimum_required(VERSION 2.8)

project(comp371project)

# Set compiler flags for g++
set(GCC_COVERAGE_COMPILE_FLAGS "-g -std=c++11 -w")
set(GCC_COVERAGE_LINK_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

# Set project directories
#set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# include_directories(include)
# include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB SOURCES "src/*.cpp")

# different library include patterns for visual studio or linux
if(MSVC)
	include_directories(${CMAKE_SOURCE_DIR}/lib/glfw ${CMAKE_SOURCE_DIR}/lib/glew)
	link_directories(${CMAKE_SOURCE_DIR}/lib/glfw ${CMAKE_SOURCE_DIR}/lib/glew)
	set(PROJECT_LINK_LIBS glew32s glu32 opengl32 glfw3)
	add_definitions(-DGLEW_STATIC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}")
else()
	find_package(glfw3 3.2 REQUIRED)
	find_package(OpenGL REQUIRED)
	set(PROJECT_LINK_LIBS GLEW GL glfw)
endif()

add_definitions(-DPROJECT_ROOT="${CMAKE_SOURCE_DIR}")

add_executable(comp371project ${SOURCES})

target_link_libraries(comp371project ${PROJECT_LINK_LIBS})