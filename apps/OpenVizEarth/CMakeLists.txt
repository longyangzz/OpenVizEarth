include_directories( ${CMAKE_CURRENT_SOURCE_DIR})
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )


file( GLOB qrc_list *.qrc )
QT5_ADD_RESOURCES( generated_qrc_list ${qrc_list} )

file( GLOB ui_list *.ui )
QT5_WRAP_UI( generated_ui_list ${ui_list})


file( GLOB header_list *.h  )
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


SETUP_APPLICATION(OpenVizEarth)
