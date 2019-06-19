
IF(DYNAMIC_DCVIZ)
    ADD_DEFINITIONS(-DOSGQWIDGET_LIB)
ENDIF()

SET(LIB_NAME OsgQWidget)
SET(HEADER_PATH ${DCViz_SOURCE_DIR}/src/${LIB_NAME})


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
    ${DCVIZ_VERSIONINFO_RC}
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
	${OSG_LIBRARY_DEBUG}
     ${OSGUTIL_LIBRARY_DEBUG}
     ${OSGDB_LIBRARY_DEBUG}
     ${OSGGA_LIBRARY_DEBUG}
     ${OPENTHREADS_LIBRARY_DEBUG}
	${OSGSHADOW_LIBRARY_DEBUG}
    ${OSGQT_LIBRARY_DEBUG}
     ${OSGANIMATION_LIBRARY_DEBUG}
     ${OSGPARTICLE_LIBRARY_DEBUG}
    ${OSGTEXT_LIBRARY_DEBUG}
	 ${OSGVIEWER_LIBRARY_DEBUG}    
	${OSGFX_LIBRARY_DEBUG} 
	 ${OSGANIMATION_LIBRARY_DEBUG} 
	 
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

