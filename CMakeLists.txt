# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

get_filename_component(TARGET_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)

project (${TARGET_NAME})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_compile_options(
		"$<$<CONFIG:RELEASE>:/O1i>"
		"$<$<CONFIG:RELEASE>:/FAs>"
		"$<$<CONFIG:RELEASE>:/GL>"
		"/EHs-"
		"$<$<CONFIG:RELEASE>:/GS->"
		"$<$<CONFIG:RELEASE>:/GR->"
	)
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
		add_compile_options("/d2noftol3")
	endif()
	add_link_options("/MAP:out.map")
endif()

# Include sub-projects.
add_subdirectory ("extern")

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

set(RES_DIR ${CMAKE_CURRENT_LIST_DIR}/resources)

set(PRIVATE_HEADER_FILES
  ${SRC_DIR}/GenTex.h
  ${SRC_DIR}/Globals.h
  ${SRC_DIR}/GLTexture.h
  ${SRC_DIR}/Math3.h
  ${SRC_DIR}/noise.h
  ${SRC_DIR}/Particles.h
  ${SRC_DIR}/utils.h
)

set(SRC_FILES
  ${SRC_DIR}/GenTex.cpp
  ${SRC_DIR}/GLTexture.cpp
  ${SRC_DIR}/Main.cpp
  ${SRC_DIR}/MATH3.CPP
  ${SRC_DIR}/noise.cpp
  ${SRC_DIR}/Particles.cpp
  ${SRC_DIR}/utils.cpp
)

set(RES_FILES
  ${RES_DIR}/windows/resource.rc
)

# Add source to this project's executable.
add_executable(${TARGET_NAME} WIN32 ${PRIVATE_HEADER_FILES} ${SRC_FILES} ${RES_FILES})
target_include_directories(${TARGET_NAME} PRIVATE ${RES_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC minifmod OpenGL::GL OpenGL::GLU)
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
