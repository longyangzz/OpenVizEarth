IF(MSVC80 OR MSVC90)
  OPTION(OSG_MSVC_GENERATE_PLUGINS_AND_WRAPPERS_MANIFESTS "Generate or not manifests files under VS8 for dynamically loaded dlls" ON)
ENDIF()

#ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)

#the old construct SUBDIRS( was substituded by ADD_SUBDIRECTORY that is to be preferred according on CMake docs.
FOREACH( mylibfolder 
        Core/propertybrowser
        Core/DC
        Core/DCDB
        Core/DCGui
      	Core/DCScene
      	Core/OsgQWidget
      	Core/Manager
      	Core/ONodeManager
        #Pyramid
        #Filters
      	plugins
      	Mplugins
      	
    )

    ADD_SUBDIRECTORY(${mylibfolder})

ENDFOREACH()

