# ------------------------------------------------------------------------------
# BOOST+CMake support for CloudCompare
# ------------------------------------------------------------------------------

OPTION( OPTION_USE_BOOST "Build with liblas support" OFF )
if( ${OPTION_USE_BOOST} )
	# Boost (using static, multithreaded libraries)
	if ( WIN32 )
		if ( MSVC )
			set(Boost_USE_STATIC_LIBS   ON)
			set(Boost_USE_MULTITHREADED ON)
		endif() 
	elseif( APPLE )
		set(Boost_USE_STATIC_LIBS   ON)
		set(Boost_USE_MULTITHREADED ON)
	endif()

	find_package(Boost ${Boost_VERSION})

	if ( Boost_FOUND AND Boost_PROGRAM_OPTIONS_FOUND )
		include_directories( ${Boost_INCLUDE_DIRS} )
		link_directories( ${Boost_LIBRARY_DIRS} ) 
	endif()

	# make these available for the user to set.
	mark_as_advanced(CLEAR Boost_INCLUDE_DIR) 
	mark_as_advanced(CLEAR Boost_LIBRARY_DIRS) 


endif()
