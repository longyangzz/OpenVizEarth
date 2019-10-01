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

		debug osg${CMAKE_DEBUG_POSTFIX}			optimized osg
	debug osgUtil${CMAKE_DEBUG_POSTFIX}		 optimized osgUtil
	debug osgDB${CMAKE_DEBUG_POSTFIX}		optimized osgDB
	debug osgGA${CMAKE_DEBUG_POSTFIX}		optimized osgGA
	debug OpenThreads${CMAKE_DEBUG_POSTFIX}	 optimized OpenThreads
	debug osgShadow${CMAKE_DEBUG_POSTFIX}	 optimized osgShadow
	debug osgQt5${CMAKE_DEBUG_POSTFIX}		optimized osgQt5
	debug osgText${CMAKE_DEBUG_POSTFIX}		optimized osgText
	debug osgViewer${CMAKE_DEBUG_POSTFIX}	 optimized osgViewer
	  ${GDAL_LIBRARY}
	  
)



ADD_SUBDIRECTORY(OpenVizViewer)
ADD_SUBDIRECTORY(OpenVizEarth)
