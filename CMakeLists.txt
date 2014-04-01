project( sap CXX )

cmake_minimum_required( VERSION 2.8.11 )

set( CMAKE_AUTOMOC true )

add_definitions( -std=c++11 )

find_package( Qt4 REQUIRED QtGui QtCore )

include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )

file( GLOB sap_SRCS "*.cxx" )

add_executable( sap ${sap_SRCS} )

target_link_libraries( sap ${QT_LIBRARIES} )

