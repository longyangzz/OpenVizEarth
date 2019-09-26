

SET(LIB_NAME AddArcGISData)
SET(HEADER_PATH ${OpenVizEarth_SOURCE_DIR}/src/Mplugins/${LIB_NAME}/scene)

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
	debug ${OSGQT_LIBRARY_DEBUG} optimized ${OSGQT_LIBRARY_RELEASE}
	debug ${OSGTEXT_LIBRARY_DEBUG} optimized ${OSGTEXT_LIBRARY}
	debug ${OSGVIEWER_LIBRARY_DEBUG} optimized ${OSGVIEWER_LIBRARY}
	OsgQWidget
	DC
	ONodeManager
	
)

SET(TARGET_LIBRARIES
    ${TARGET_LIBRARIES}
    debug M_EarthDataInterfaced optimized M_EarthDataInterface
)

INCLUDE_DIRECTORIES(
		${QT_INCLUDE_DIR} 
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${OSG_INCLUDE_DIR}
		${GDAL_INCLUDE_DIR}
	)
message("6666666666666666666666666666${MPlugin_LIBRARY_OUTPUT_DIRECTORY_DEBUG}")
LINK_DIRECTORIES(${OSG_DIR/lib} )
link_libraries(debug "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}/M_EarthDataInterfaced.lib" optimized "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}/M_EarthDataInterface.lib")
SETUP_MPLUGIN(${LIB_NAME})


