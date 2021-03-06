cmake_minimum_required (VERSION 2.8)
project (Sigma)

# Set the include directory
include_directories("${CMAKE_SOURCE_DIR}/include")

# Set the cmake search paths
set(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)

# change output directory of executable so that relative paths in the code are correct
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/bin)

#Setup the core variables
set(Sigma_SRC "")
set(Sigma_LIB "")

# Set the directory where to find FindSLD2.cmake
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

# find all source files in the src directory
file(GLOB Sigma_SRC "include/*.h" "src/*.cpp")

# Filter the source/header files for various groups of files
# Put all component files into a filter
file(GLOB Sigma_SRC_COMPONENT_H "include/components/*.h")
source_group("Header Files\\components" FILES ${Sigma_SRC_COMPONENT_H})
file(GLOB Sigma_SRC_COMPONENT_CPP "src/components/*.cpp")
source_group("Source Files\\components" FILES ${Sigma_SRC_COMPONENT_CPP})

# Put all Systems files into a filter
file(GLOB Sigma_SRC_SYSTEM_H "include/systems/*.h")
source_group("Header Files\\systems" FILES ${Sigma_SRC_SYSTEM_H})
file(GLOB Sigma_SRC_SYSTEM_CPP "src/systems/*.cpp")
source_group("Source Files\\systems" FILES ${Sigma_SRC_SYSTEM_CPP})

# Put all Controller files into a filter
file(GLOB Sigma_SRC_CONTROLLERS_H "include/controllers/*.h")
source_group("Header Files\\controllers" FILES ${Sigma_SRC_CONTROLLERS_H})
file(GLOB Sigma_SRC_CONTROLLERS_CPP "src/controllers/*.cpp")
source_group("Source Files\\controllers" FILES ${Sigma_SRC_CONTROLLERS_CPP})

# Outpit the compiler ID.
message("Compiler ID: " ${CMAKE_CXX_COMPILER_ID})

# check for gcc version to set c++11 or c++0x
# thanks to http://stackoverflow.com/questions/10984442/how-to-detect-c11-support-of-a-compiler-with-cmake
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        # c++11 std only appears in gcc 4.7 or higher. c++0x will work for lesser versions.
        message("gcc ${GCC_VERSION} does not support c++11, only c++0x. This may cause some problems in the future. We recommend upgrading to gcc-4.7 or greater.")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    else(NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        # compiler is gcc 4.7 or higher. Force c++11.
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
elseif (MSVC AND (MSVC10 OR MSVC11))
    message("Support Visual Studio!")
else ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")

add_definitions(
	-DGLEW_STATIC # Add a define to prevent GLEW linker warnings
)

## Set the libraries

# Link aganst OpenGL and GLEW
# We can use the find OpenGL package on windows
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
	message(FATAL_ERROR "OpenGL not found! Aborting!")
endif(NOT OPENGL_FOUND)

# Figure out what Operating System to compile against
if(WIN32 AND NOT MINGW) # Windows
	set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/include/libraries)
	include_directories("${CMAKE_SOURCE_DIR}/include/libraries")
	set(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/lib/x86)
	link_directories("${CMAKE_SOURCE_DIR}/lib/x86")
	
	set(GLEW_LIBRARY glew32s glew32)
	set(BULLET_LINEARMATH_LIBRARIES LinearMath)
	set(BULLET_COLLISION_LIBRARIES BulletCollision)
	set(BULLET_DYNAMICS_LIBRARIES BulletDynamics)
	SET(SOIL_LIBRARY SOIL)
	set(IOPSYS "OS_Win32" CACHE STRING "Select the Op Sys you want to use")
endif(WIN32 AND NOT MINGW)

if(MINGW OR UNIX OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux") # *nix compliant
	find_package(SDL2 REQUIRED)

	find_package(GLEW REQUIRED)
	if(NOT GLEW_FOUND)
		message(FATAL_ERROR "GLEW not found! Aborting!")
	endif(NOT GLEW_FOUND)

	find_package(Bullet REQUIRED)
	if(NOT (BULLET_DYNAMICS_FOUND AND BULLET_COLLISION_FOUND AND BULLET_LINEARMATH_FOUND))
		message(FATAL_ERROR "Bullet Physics was not found. Aborting!")
	endif(NOT (BULLET_DYNAMICS_FOUND AND BULLET_COLLISION_FOUND AND BULLET_LINEARMATH_FOUND))
	
	find_package(SOIL REQUIRED)
	if(NOT ${SOIL_FOUND})
		message(FATAL_ERROR "SOIL wasn't found!")
		message(ERROR "Please install SOIL, or if you have it installed, make sure that modules/FindSOIL.cmake looks for it")
	endif(NOT ${SOIL_FOUND})

	# Link the libraries
	set(Sigma_LIB ${Sigma_LIB} ${SDL2_LIBRARY} ${BULLET_LINEARMATH_LIBRARY} ${BULLET_COLLISION_LIBRARY} ${BULLET_DYNAMICS_LIBRARY})

	set(IOPSYS "OS_SDL" CACHE STRING "Select the Op Sys you want to use")
endif(MINGW OR UNIX OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(APPLE) # Mac OSX
  #TODO: Add stuff here if needed
endif(APPLE)

# Add a cache line to allow choosing between IOpSys
set_property(CACHE IOPSYS PROPERTY STRINGS "OS_SDL" "OS_Win32")

#Add in our IOpSys
message("Using ${IOPSYS}...")
add_subdirectory("src/os")

# add the executable
add_executable(Sigma ${Sigma_SRC} ${Sigma_SRC_COMPONENT_H} ${Sigma_SRC_COMPONENT_CPP} ${Sigma_SRC_SYSTEM_H} ${Sigma_SRC_SYSTEM_CPP} ${Sigma_SRC_CONTROLLERS_H} ${Sigma_SRC_CONTROLLERS_CPP})

# Set the preprocessor define for the IOpSys to use.
add_definitions(-D${IOPSYS})

# Link the executable to the Hello library.
target_link_libraries (Sigma ${Sigma_LIB} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY} ${SOIL_LIBRARY} ${BULLET_LINEARMATH_LIBRARIES} ${BULLET_COLLISION_LIBRARIES} ${BULLET_DYNAMICS_LIBRARIES})
