PROJECT(OpenVIZ_APPS)

SET(OpenVIZCORE_BUNDLED TRUE)

SET(OpenVIZ_APPLICATION_DIR ${PROJECT_SOURCE_DIR})

find_package(OsgEarth 2.9 REQUIRED osgEarthFeatures osgEarthUtil osgEarthSymbology osgEarthAnnotation)

INCLUDE_DIRECTORIES(
		${QT_INCLUDE_DIR} 
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${OSG_INCLUDE_DIR}
		${OSGEARTH_INCLUDE_DIRS}
    ${OSGEARTH_INCLUDE_DIR}
    ${GDAL_INCLUDE_DIR}
	)

SET(TARGET_DEFAULT_PREFIX "application_")
SET(TARGET_DEFAULT_LABEL_PREFIX "Applications")
SET(TARGET_COMMON_LIBRARIES 
    DC
		DCGui
		DCScene
		Manager
		DCDB
		ONodeManager
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
	  ${GDAL_LIBRARY}
	  
)



ADD_SUBDIRECTORY(OpenVizViewer)
ADD_SUBDIRECTORY(OpenVizEarth)
