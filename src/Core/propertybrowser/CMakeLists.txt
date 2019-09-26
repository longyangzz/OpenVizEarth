
IF(DYNAMIC_OPENVIZ)
    ADD_DEFINITIONS(-DQT_QTPROPERTYBROWSER_EXPORT)
	ADD_DEFINITIONS(-DQ_WS_WIN)
ENDIF()

SET(LIB_NAME Propertybrowser)
SET(HEADER_PATH ${OpenVizEarth_SOURCE_DIR}/src/Core/${LIB_NAME})

SET(SOURCES_H
    ${HEADER_PATH}/qtbuttonpropertybrowser.h
    ${HEADER_PATH}/qteditorfactory.h
    ${HEADER_PATH}/qtgroupboxpropertybrowser.h
    ${HEADER_PATH}/qtpropertybrowser.h
    ${HEADER_PATH}/qtpropertybrowserutils_p.h
    ${HEADER_PATH}/qtpropertymanager.h
    ${HEADER_PATH}/qttreepropertybrowser.h
    ${HEADER_PATH}/qtvariantproperty.h
)

#QT5_WRAP_CPP( SOURCES_H_MOC ${SOURCES_H} OPTIONS "-f" )
#qt5_wrap_ui( generated_ui_list ${ui_list} )
#qt5_add_resources( generated_qrc_list ${qrc_list} )

SET(TARGET_H
    ${HEADER_PATH}/qtbuttonpropertybrowser.h
    ${HEADER_PATH}/qteditorfactory.h
    ${HEADER_PATH}/qtgroupboxpropertybrowser.h
    ${HEADER_PATH}/qtpropertybrowser.h
    ${HEADER_PATH}/qtpropertybrowserutils_p.h
    ${HEADER_PATH}/qtpropertymanager.h
    ${HEADER_PATH}/qttreepropertybrowser.h
    ${HEADER_PATH}/qtvariantproperty.h
)


SET(TARGET_SRC
    qtbuttonpropertybrowser.cpp
    qteditorfactory.cpp
    qtgroupboxpropertybrowser.cpp
    qtpropertybrowser.cpp
    qtpropertybrowserutils.cpp
    qtpropertymanager.cpp
    qttreepropertybrowser.cpp
    qtvariantproperty.cpp
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
	#Qt5::Xml
)

# FIXME: This should work but something with the LINK_WITH_VARIABLES macro is not working
#SET(TARGET_LIBRARIES_VARS
#    QT_QTCORE_LIBRARY 
#    QT_QTGUI_LIBRARY
#    QT_QTOPENGL_LIBRARY
#)
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR})

SETUP_LIBRARY(${LIB_NAME})

