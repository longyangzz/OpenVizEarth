
IF(DYNAMIC_OPENVIZ)
    ADD_DEFINITIONS(-DPYRAMID_EXPORT)
ENDIF()

SET(LIB_NAME Pyramid)
SET(HEADER_PATH ${OpenVizEarth_SOURCE_DIR}/src/Core/${LIB_NAME})

SET(SOURCES_H
   # ${HEADER_PATH}/ObjectLoader.h
)

#QT5_WRAP_CPP( SOURCES_H_MOC ${SOURCES_H} OPTIONS "-f" )

## method 2
file( GLOB header_list *.h  )
SET(TARGET_H ${header_list})

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
    optimized ${OSG_LIBRARY} debug ${OSG_LIBRARY_DEBUG}
    optimized ${OSGUTIL_LIBRARY} debug ${OSGUTIL_LIBRARY_DEBUG}
    optimized ${OSGDB_LIBRARY} debug ${OSGDB_LIBRARY_DEBUG}
    ${LIBLAS_RELEASE_LIBRARY_FILE}
    OsgQWidget 
    DC
)


INCLUDE_DIRECTORIES(
        ${QT_INCLUDE_DIR} 
        ${QT_QTCORE_INCLUDE_DIR}
        ${QT_QTGUI_INCLUDE_DIR}
        ${OSG_INCLUDE_DIR}
    )

SETUP_LIBRARY(${LIB_NAME})

