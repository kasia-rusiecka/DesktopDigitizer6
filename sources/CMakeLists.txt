#--- CMakeLists.txt for DD6 package.
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(DesktopDigitizer6)


#--- You need to tell CMake where to find the ROOT installation.
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})


#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Spectrum)


#---Set global variables e.g. where sources and includes are
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
ROOT_GENERATE_DICTIONARY(G__DesktopDigitizer6 ${headers} LINKDEF digitLinkDef.h)


#---Create a shared library with generated dictionary
add_library(DesktopDigitizer6 SHARED digit.cc ${sources} G__DesktopDigitizer6.cxx)
target_link_libraries(DesktopDigitizer6 ${ROOT_LIBRARIES})


#---Create  a main program using the library
add_executable(digit digit.cc)
target_link_libraries(digit DesktopDigitizer6)

add_executable(calib calib.cc)
target_link_libraries(calib DesktopDigitizer6)


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