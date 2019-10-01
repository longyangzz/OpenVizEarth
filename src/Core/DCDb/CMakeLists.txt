
IF(DYNAMIC_OPENVIZ)
    ADD_DEFINITIONS(-DDCDB_LIB)
ENDIF()

SET(LIB_NAME DCDB)
SET(HEADER_PATH ${OpenVizEarth_SOURCE_DIR}/src/Core/${LIB_NAME})

SET(SOURCES_H
    ${HEADER_PATH}/ObjectLoader.h
)

#QT5_WRAP_CPP( SOURCES_H_MOC ${SOURCES_H} OPTIONS "-f" )
#qt5_wrap_ui( generated_ui_list ${ui_list} )
#qt5_add_resources( generated_qrc_list ${qrc_list} )

## method 2
file( GLOB header_list *.h  )
SET(TARGET_H ${header_list})

## method 1
#SET(TARGET_H
#    ${HEADER_PATH}/AppSettings.h
#    ${HEADER_PATH}/dc_global.h
#   ${HEADER_PATH}/LogHandler.h
#    ${HEADER_PATH}/OsgLogger.h
#    ${HEADER_PATH}/ThreadPool.h
#)

file( GLOB source_list *.cpp  )
SET(TARGET_SRC
    ${source_list}
    ${SOURCES_H_MOC}
    ${OpenVIZ_VERSIONINFO_RC}
)


IF(CMAKE_COMPILER_IS_GNUCXX)
    # Remove -pedantic flag as it barfs on Qt headers
   STRING(REGEX REPLACE "-pedantic" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ENDIF()

SET(TARGET_LIBRARIES
    ${TARGET_LIBRARIES}
    Qt5::Core
	Qt5::Gui 
	Qt5::Widgets 
    debug osgEarth${CMAKE_DEBUG_POSTFIX}						optimized osgEarth
    debug osgEarthAnnotation${CMAKE_DEBUG_POSTFIX}			optimized osgEarthAnnotation
	debug osgEarthFeatures${CMAKE_DEBUG_POSTFIX}				optimized osgEarthFeatures
    debug osgEarthUtil${CMAKE_DEBUG_POSTFIX}					optimized osgEarthUtil
	debug osgEarthSymbology${CMAKE_DEBUG_POSTFIX}			optimized osgEarthSymbology
    debug osgSim${CMAKE_DEBUG_POSTFIX}						optimized osgSim

	debug osg${CMAKE_DEBUG_POSTFIX}			optimized osg
	debug osgUtil${CMAKE_DEBUG_POSTFIX}		 optimized osgUtil
	debug osgDB${CMAKE_DEBUG_POSTFIX}		optimized osgDB
	debug osgGA${CMAKE_DEBUG_POSTFIX}		optimized osgGA
	debug OpenThreads${CMAKE_DEBUG_POSTFIX}	 optimized OpenThreads
	debug osgShadow${CMAKE_DEBUG_POSTFIX}	 optimized osgShadow
	debug osgQt5${CMAKE_DEBUG_POSTFIX}		optimized osgQt5
	debug osgText${CMAKE_DEBUG_POSTFIX}		optimized osgText
	debug osgViewer${CMAKE_DEBUG_POSTFIX}	 optimized osgViewer
	 OsgQWidget
	 DC
    
)

# FIXME: This should work but something with the LINK_WITH_VARIABLES macro is not working
#SET(TARGET_LIBRARIES_VARS
#    QT_QTCORE_LIBRARY 
#    QT_QTGUI_LIBRARY
#    QT_QTOPENGL_LIBRARY
#)
INCLUDE_DIRECTORIES(
		${QT_INCLUDE_DIR} 
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${OSG_INCLUDE_DIR}
	)

SETUP_LIBRARY(${LIB_NAME})

