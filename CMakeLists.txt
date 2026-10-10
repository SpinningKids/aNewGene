# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

get_filename_component(TARGET_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)

project (${TARGET_NAME})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_compile_options(
		"$<$<CONFIG:RELEASE>:/O1i>"
		"$<$<CONFIG:RELEASE>:/GL>"
		"$<$<CONFIG:RELEASE>:/Gy>"
		"$<$<CONFIG:RELEASE>:/GF>"
		"/EHs-"
		"$<$<CONFIG:RELEASE>:/GS->"
		"$<$<CONFIG:RELEASE>:/GR->"
	)
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
		add_compile_options("/d2noftol3")
	endif()
endif()

# Include sub-projects.
add_subdirectory ("extern")

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

if(NOT WIN32)
	find_package(SDL2 REQUIRED)
endif()

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

set(RES_DIR ${CMAKE_CURRENT_LIST_DIR}/Resources)

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
target_link_libraries(${TARGET_NAME} PUBLIC minifmod OpenGL::GL OpenGL::GLU $<$<NOT:$<PLATFORM_ID:Windows>>:SDL2::SDL2>)
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)

# Map files: one unique file per target. A single relative /MAP path shared by
# all targets in a build tree breaks the second link (map files come out
# read-only) and so does minifmod's own global /MAP:out.map. The last /MAP
# option on the command line wins, so these per-target options override it.
if(MSVC)
	# minifmod's add_link_options() (/LTCG /OPT:ICF) is scoped to its own
	# directory and never reaches this target; without a /LTCG link the
	# /GL compile flags are a no-op, so add them explicitly for release.
	set_property(TARGET ${TARGET_NAME} APPEND PROPERTY
		LINK_OPTIONS
		"$<$<CONFIG:RELEASE>:/LTCG>"
		"$<$<CONFIG:RELEASE>:/OPT:ICF>"
		"$<$<CONFIG:RELEASE>:/OPT:REF>")
	set_property(TARGET ${TARGET_NAME} APPEND PROPERTY LINK_OPTIONS "/MAP:$<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.map")
	foreach(exe minifmod-example minixm-example)
		if(TARGET ${exe})
			set_property(TARGET ${exe} APPEND PROPERTY LINK_OPTIONS "/MAP:$<TARGET_FILE_DIR:${exe}>/${exe}.map")
		endif()
	endforeach()
endif()
