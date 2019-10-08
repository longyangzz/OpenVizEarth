
IF(DYNAMIC_OPENVIZ)
    ADD_DEFINITIONS(-DQT_QTPROPERTYBROWSER_EXPORT)
	ADD_DEFINITIONS(-DQ_WS_WIN)
ENDIF()

SET(LIB_NAME Propertybrowser)
SET(HEADER_PATH ${OpenVizEarth_SOURCE_DIR}/src/Core/${LIB_NAME})


#QT5_WRAP_CPP( SOURCES_H_MOC ${SOURCES_H} OPTIONS "-f" )
#qt5_wrap_ui( generated_ui_list ${ui_list} )
#qt5_add_resources( generated_qrc_list ${qrc_list} )
FIND_PACKAGE(Qt5LinguistTools)
file(GLOB ts_list *.ts)

set(TS_FILES
    ${OpenVizEarth_SOURCE_DIR}/src/Core/${LIB_NAME}/${LIB_NAME}.ts
)
file(GLOB_RECURSE TS_SOURCES "*.cpp" "*.h" "*.ui")
#add_custom_target(translations)
foreach(_ts_file ${TS_FILES})
    # generate a sensible name for this translation file
    get_filename_component(_ts_name ${_ts_file} NAME_WE)
    # call lupdate ourselves
    add_custom_command(
        OUTPUT "${_ts_file}"
        DEPENDS ${TS_SOURCES}
        COMMAND ${Qt5_LUPDATE_EXECUTABLE} -recursive ${CMAKE_CURRENT_SOURCE_DIR} -ts ${_ts_file}
    )
    # create a target that depends on the generated files
	add_custom_target(translate_${_ts_name} DEPENDS ${_ts_file})
    # attach the custom target for this ts file to the parent target
    #add_dependencies(translations translate_${_ts_name})
endforeach()

# generate qm files
QT5_ADD_TRANSLATION(QM_FILES ${TS_FILES})
message("3333333333333333333333")
message(${QM_FILES})
message(${TS_FILES})
## method 2
file( GLOB header_list *.h  )
SET(TARGET_H ${header_list})


file( GLOB cpp_list *.cpp *.ts impl/*.cpp impl/*.cxx )
SET(TARGET_SRC
	
	${cpp_list}
    ${SOURCES_H_MOC}
    ${OpenVIZ_VERSIONINFO_RC}
)

#FIND_PACKAGE(Qt5LinguistTools)
#file(GLOB ts_list *.ts)
# QT5_ADD_TRANSLATION£¨QM_FILES $ {ts_list}£©

#ADD_CUSTOM_TARGET(ts_append COMMAND QT5_CREATE_TRANSLATION(QM_FILES ${CMAKE_SOURCE_DIR}/src/Core/${LIB_NAME}/${ts_list}) )



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

