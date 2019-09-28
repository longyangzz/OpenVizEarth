
IF(DYNAMIC_OPENVIZ)
    ADD_DEFINITIONS(-DDATAMANAGER_LIB)
ENDIF()

SET(LIB_NAME ONodeManager)
SET(HEADER_PATH ${OpenVizEarth_SOURCE_DIR}/src/Core/${LIB_NAME})

file( GLOB ui_list *.ui )
QT5_WRAP_UI( generated_ui_list ${ui_list})

## method 2
file( GLOB header_list *.h  )
SET(TARGET_H 
	${header_list}
	${generated_ui_list}
)


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
	Qt5::OpenGL 
	DC
	OsgWidget
	DCScene
	debug osgEarth${CMAKE_DEBUG_POSTFIX}						optimized osgEarth
    debug osgEarthAnnotation${CMAKE_DEBUG_POSTFIX}			optimized osgEarthAnnotation
	debug osgEarthFeatures${CMAKE_DEBUG_POSTFIX}				optimized osgEarthFeatures
    debug osgEarthUtil${CMAKE_DEBUG_POSTFIX}					optimized osgEarthUtil
	debug osgEarthSymbology${CMAKE_DEBUG_POSTFIX}			optimized osgEarthSymbology
    debug osgSim${CMAKE_DEBUG_POSTFIX}						optimized osgSim

	debug ${OSG_LIBRARY_DEBUG} optimized ${OSG_LIBRARY}
	debug ${OSGUTIL_LIBRARY_DEBUG} optimized ${OSGUTIL_LIBRARY}
	debug ${OSGDB_LIBRARY_DEBUG} optimized ${OSGDB_LIBRARY}
	debug ${OSGGA_LIBRARY_DEBUG} optimized ${OSGGA_LIBRARY}
	debug ${OPENTHREADS_LIBRARY_DEBUG} optimized ${OPENTHREADS_LIBRARY}
	debug ${OSGSHADOW_LIBRARY_DEBUG} optimized ${OSGSHADOW_LIBRARY}
	debug ${OSGQT_LIBRARY_DEBUG} optimized ${OSGQT_LIBRARY}
	debug ${OSGTEXT_LIBRARY_DEBUG} optimized ${OSGTEXT_LIBRARY}
	debug ${OSGVIEWER_LIBRARY_DEBUG} optimized ${OSGVIEWER_LIBRARY}
	 
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

