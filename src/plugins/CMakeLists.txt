PROJECT(OpenViz_PLUGINS)


SET(CMAKE_SHARED_MODULE_PREFIX ${OSG_PLUGIN_PREFIX})

SET(TARGET_DEFAULT_PREFIX "osgdb_")
SET(TARGET_DEFAULT_LABEL_PREFIX "Plugins")


############################################################
#
#  selfdefine loader plugins
#
ADD_SUBDIRECTORY(EarthDataInterface)



