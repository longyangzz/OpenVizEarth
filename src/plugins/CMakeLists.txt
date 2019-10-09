PROJECT(OpenViz_PLUGINS)


SET(CMAKE_SHARED_MODULE_PREFIX ${OSG_PLUGIN_PREFIX})

SET(TARGET_DEFAULT_PREFIX "osgdb_")
SET(TARGET_DEFAULT_LABEL_PREFIX "Plugins")

FOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})        # For each configuration (Debug, Release, MinSizeRel... and/or anything the user chooses)
    STRING(TOUPPER "${CONF}" CONF)                # Go uppercase (DEBUG, RELEASE...)

    message("${OUTPUT_BINDIR}/${CONF}/${TARGET_DEFAULT_LABEL_PREFIX}")
    IF(WIN32)
        SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}/${CONF}/${TARGET_DEFAULT_LABEL_PREFIX}")
    ELSE()
        SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}/${CONF}/${TARGET_DEFAULT_LABEL_PREFIX}")
    ENDIF()
ENDFOREACH()

############################################################
#
#  selfdefine loader plugins
#
ADD_SUBDIRECTORY(OsgIOPluginDemo)
ADD_SUBDIRECTORY(osgdb_s3c)




