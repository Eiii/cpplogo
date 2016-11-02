file( GLOB_RECURSE DRIVER_SOURCES "*.cc" )

add_executable( test_cpplogo ${DRIVER_SOURCES} )
target_link_libraries( test_cpplogo ${LIB_NAME} )
