# https://github.com/mauriciopoppe/cmake-starter/blob/master/CMakeLists.txt

########## Compiler Setup ##########

# 1. Setup: Declare min CMake version, name of the project, and compiler settings
cmake_minimum_required( VERSION 3.0 )
project( isaac_ros_service )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -lboost_system -pthread" ) # Use C++17
# NOTE: CMake function calls do NOT have commas between arguments!


########## Project Setup ##########

# 2. Set paths to look for headers and sources in the project
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/out" )
set( EXM_DIR ${PROJECT_SOURCE_DIR}/example )
set( SRC_DIR ${PROJECT_SOURCE_DIR}/src     )
set( TST_DIR ${PROJECT_SOURCE_DIR}/test    )
set( HLP_DIR ${PROJECT_SOURCE_DIR}/helpers )
# NOTE: Use ${VAR_NAME} to substitute the value of `VAR_NAME` in that place

# the -I flag in gcc
include_directories(  
  ${EXM_DIR}
  ${SRC_DIR}
  ${TST_DIR}
  ${HLP_DIR}
) 

# 3. Shortcuts
# set( HELP_CPP ${SRC_DIR}/demo2400_helpers.cpp )


########## Libraries ##########

set( THREADS_PREFER_PTHREAD_FLAG ON )
find_package( Threads REQUIRED )

set( BOOST_VRSN_MIN 1.65.1 )

set( Boost_USE_STATIC_LIBS    OFF ) 
set( Boost_USE_MULTITHREADED  ON  )  
set( Boost_USE_STATIC_RUNTIME OFF ) 

set( ASIO_HDR_DIR ${PROJECT_SOURCE_DIR}/boost_asio/include/boost )

# set( BOOST_ROOT /usr/lib/x86_64-linux-gnu/ )
set( BOOST_LIBRARYDIR /usr/lib/x86_64-linux-gnu )

find_package( Boost ${BOOST_VRSN_MIN} REQUIRED COMPONENTS 
    system
    filesystem
    thread
    chrono
) 

if( Boost_FOUND )
    include_directories( 
        ${Boost_INCLUDE_DIRS} 
        ${ASIO_HDR_DIR}
    )
    link_directories( ${Boost_LIBRARY_DIRS} )
else() 
    message( FATAL_ERROR "Fatal error: Boost (version >= ${BOOST_VRSN_MIN}) required.\n")
endif()


########## Build Info ##########

# 4. Some messages to the user to let them know how things are going
message( "Project Name: ${PROJECT_NAME}" )
message( "~~ Directories ~~" )
message( "Examples: ${EXM_DIR}" )
message( "Sources:  ${SRC_DIR}" )
message( "Programs: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )


########## Programs & Libraries ##########

# 5. Add executable programs to the project

message( "\n\nWriting build scripts..." )

##### Examples #####

set( EXEC_NAME server_blk )
add_executable( ${EXEC_NAME} ${EXM_DIR}/${EXEC_NAME}.cpp )
target_link_libraries( ${EXEC_NAME} 
  ${Boost_LIBRARIES}
)
message( "${EXEC_NAME} ready!" )

set( EXEC_NAME client_blk )
add_executable( ${EXEC_NAME} ${EXM_DIR}/${EXEC_NAME}.cpp )
target_link_libraries( ${EXEC_NAME} 
  ${Boost_LIBRARIES}
)
message( "${EXEC_NAME} ready!" )

set( EXEC_NAME thread_pool )
add_executable( ${EXEC_NAME} ${EXM_DIR}/${EXEC_NAME}.cpp )
target_link_libraries( ${EXEC_NAME} 
  ${Boost_LIBRARIES}
)
message( "${EXEC_NAME} ready!" )

set( EXEC_NAME lockfree_queue )
add_executable( ${EXEC_NAME} ${EXM_DIR}/${EXEC_NAME}.cpp )
target_link_libraries( ${EXEC_NAME} 
  ${Boost_LIBRARIES}
)
message( "${EXEC_NAME} ready!" )

set( EXEC_NAME lock_guard )
add_executable( ${EXEC_NAME} ${EXM_DIR}/${EXEC_NAME}.cpp )
target_link_libraries( ${EXEC_NAME} 
  ${Boost_LIBRARIES}
)
message( "${EXEC_NAME} ready!" )

##### Tests #####

set( EXEC_NAME shared )
add_executable( ${EXEC_NAME} ${TST_DIR}/${EXEC_NAME}.cpp ${HLP_DIR}/helper.cpp )
target_link_libraries( ${EXEC_NAME} 
  ${Boost_LIBRARIES}
)
message( "${EXEC_NAME} ready!" )

# set( EXEC_NAME test01_server )
# add_executable( ${EXEC_NAME} ${TST_DIR}/${EXEC_NAME}.cpp ${HLP_DIR}/helper.cpp ${SRC_DIR}/Connection.cpp ${SRC_DIR}/IP.cpp )
# target_link_libraries( ${EXEC_NAME} 
#   ${Boost_LIBRARIES}
#   ${CMAKE_THREAD_LIBS_INIT}
# )
# message( "${EXEC_NAME} ready!" )



message( "\n\n Cmake setup COMPLETED" )