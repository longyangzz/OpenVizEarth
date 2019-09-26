
IF(DYNAMIC_OPENVIZ)
    ADD_DEFINITIONS(-DDCSCENE_LIB)
ENDIF()

SET(LIB_NAME DCScene)
SET(HEADER_PATH ${OpenVizEarth_SOURCE_DIR}/src/Core/${LIB_NAME}/scene)


## method 2
file( GLOB header_list scene/*.h  )
SET(TARGET_H ${header_list})

file( GLOB source_list scene/*.cpp  )
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
	OsgQWidget
	DC
	debug osg${CMAKE_DEBUG_POSTFIX}						optimized osg
    debug osgUtil${CMAKE_DEBUG_POSTFIX}					optimized osgUtil
	debug osgDB${CMAKE_DEBUG_POSTFIX}					optimized osgDB
    debug osgGA${CMAKE_DEBUG_POSTFIX}					optimized osgGA
	debug OpenThreads${CMAKE_DEBUG_POSTFIX}				optimized OpenThreads
    debug osgShadow${CMAKE_DEBUG_POSTFIX}				optimized osgShadow
	debug osgQt5${CMAKE_DEBUG_POSTFIX}					optimized osgQt5
	debug osgText${CMAKE_DEBUG_POSTFIX}					optimized osgText
    debug osgViewer${CMAKE_DEBUG_POSTFIX}				optimized osgViewer
	debug osgFX${CMAKE_DEBUG_POSTFIX}					optimized osgFX
    debug osgSim${CMAKE_DEBUG_POSTFIX}					optimized osgSim
)


INCLUDE_DIRECTORIES(
		${QT_INCLUDE_DIR} 
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${OSG_INCLUDE_DIR}
	)

LINK_DIRECTORIES(${OSG_DIR/lib})

SETUP_LIBRARY(${LIB_NAME})

