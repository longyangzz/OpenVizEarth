
IF(DYNAMIC_OPENVIZ)
    ADD_DEFINITIONS(-DEARTHDATAINTERFACE_LIB)
ENDIF()

SET(LIB_NAME EarthDataInterface)
SET(HEADER_PATH ${OpenVizEarth_SOURCE_DIR}/src/plugins/${LIB_NAME}/scene)

file( GLOB qrc_list *.qrc )
QT5_ADD_RESOURCES( generated_qrc_list ${qrc_list} )

file( GLOB ui_list *.ui )
QT5_WRAP_UI( generated_ui_list ${ui_list})


file( GLOB header_list *.h *.json  )
SET(TARGET_H 
	${header_list}
	${generated_ui_list}
	${OpenVizEarth_SOURCE_DIR}/NameSpace.h
)


file( GLOB cpp_list *.cpp impl/*.cpp impl/*.cxx )
SET(TARGET_SRC
	${cpp_list} 
    ${SOURCES_H_MOC}
	${generated_qrc_list}
    ${OpenVIZ_VERSIONINFO_RC}
)

SET(TARGET_COMMON_LIBRARIES
    ${TARGET_COMMON_LIBRARIES}
    Qt5::Core
	Qt5::Gui 
	Qt5::Widgets 
	${GDAL_LIBRARY}
    ${OSG_LIBRARY_DEBUG}
    ${OSGUTIL_LIBRARY_DEBUG}
    ${OSGDB_LIBRARY_DEBUG}
     ${OSGGA_LIBRARY_DEBUG}
    ${OPENTHREADS_LIBRARY_DEBUG}
	${OSGSHADOW_LIBRARY_DEBUG}
    ${OSGQT_LIBRARY_DEBUG}
    ${OSGTEXT_LIBRARY_DEBUG}
	${OSGVIEWER_LIBRARY_DEBUG}
	OsgQWidget
	DC
	ONodeManager
)

INCLUDE_DIRECTORIES(
		${QT_INCLUDE_DIR} 
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${OSG_INCLUDE_DIR}
		${GDAL_INCLUDE_DIR}
	)

LINK_DIRECTORIES(${OSG_DIR/lib})

SETUP_PLUGIN(${LIB_NAME})

