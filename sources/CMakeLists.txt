#--- CMakeLists.txt for DD6 package.
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(DesktopDigitizer6)


set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_PATCH 1)
set(PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

include(GNUInstallDirs)

#--- You need to tell CMake where to find the ROOT installation.
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})


#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Spectrum)


#---Set global variables e.g. where sources and includes are
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/include)
ROOT_GENERATE_DICTIONARY(G__DesktopDigitizer6 ${headers} LINKDEF digitLinkDef.h)


#---Set paths
if(WIN32 AND NOT CYGWIN)
	set(DEF_CMAKE_INSTALL_CMAKEDIR cmake)
else()
	set(DEF_CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
endif()

set(CMAKE_INSTALL_CMAKEDIR ${DEF_CMAKE_INSTALL_CMAKEDIR} CACHE PATH "Installation directory for CMake files")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


#---Create a shared library with generated dictionary
add_library(DesktopDigitizer6 SHARED digit.cc ${sources} G__DesktopDigitizer6.cxx)
target_link_libraries(DesktopDigitizer6 ${ROOT_LIBRARIES})


#---Create  a main program using the library
add_executable(digit digit.cc)
target_link_libraries(digit DesktopDigitizer6)

add_executable(calib calib.cc)
target_link_libraries(calib DesktopDigitizer6)

set_target_properties(DesktopDigitizer6 PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${VERSION_MAJOR}
	PUBLIC_HEADER "${headers}"
	INTERFACE_${CMAKE_PROJECT_NAME}_MAJOR_VERSION ${VERSION_MAJOR}
	)

#---Installation
install(TARGETS DesktopDigitizer6
	EXPORT ${CMAKE_PROJECT_NAME}Targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libDesktopDigitizer6_rdict.pcm
	DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
#install(TARGETS DesktopDigitizer6Dict
#	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#)

install(TARGETS digit calib
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ${CMAKE_PROJECT_NAME})

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_SOURCE_DIR}/Config.cmake.in ${CMAKE_PROJECT_NAME}Config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
	PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_BINDIR)

write_basic_package_version_file(
	${CMAKE_PROJECT_NAME}ConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

export(EXPORT ${CMAKE_PROJECT_NAME}Targets
	FILE ${CMAKE_PROJECT_NAME}/${CMAKE_PROJECT_NAME}Targets.cmake
)

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
	FILE ${CMAKE_PROJECT_NAME}Targets.cmake
	DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)


#---Locating Doxygen and generating documentation 
find_package(Doxygen)

if(DOXYGEN_FOUND)
	set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/../sources/Doxyfile.in)
	set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

	configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
	message("Doxygen build started")

	add_custom_target(doc_doxygen ALL
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM)
else(DOXYGEN_FOUND)
	message("Deoxygen must be installed to generate the documentation")
endif(DOXYGEN_FOUND)
