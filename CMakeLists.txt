set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)

IF(COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(OpenVizEarth)
#cmake_policy(SET CMP0071 OLD)
SET(OpenVIZ_MAJOR_VERSION 1)
SET(OpenVIZ_MINOR_VERSION 0)
SET(OpenVIZ_PATCH_VERSION 0)
SET(OpenVIZ_SOVERSION 10)

SET(OpenVIZ_VERSION ${OpenVIZ_MAJOR_VERSION}.${OpenVIZ_MINOR_VERSION}.${OpenVIZ_PATCH_VERSION})

SET(OpenVIZ_PLUGINS osgPlugins-${SOVERSION_VERSION})

SET(OpenVIZ_PLUGIN_PREFIX "")

# We want to build SONAMES shared librariess
SET(OpenVIZ_SONAMES TRUE)


# We have some custom .cmake scripts not in the official distribution.
# Maybe this can be used override existing behavior if needed?
SET(CMAKE_MODULE_PATH "${OpenVizEarth_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

INCLUDE_DIRECTORIES(
    ${OpenVizEarth_SOURCE_DIR}/src
    ${OPENGL_INCLUDE_DIR}
)


################################################################################
# Set Version Info resource file

IF(MSVC)
    SET(OpenVIZ_VERSIONINFO_RC "${PROJECT_BINARY_DIR}/PlatformSpecifics/Windows/OpenVizVersionInfo.rc")
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/OpenVizVersionInfo.rc.in"
                   "${OpenVIZ_VERSIONINFO_RC}")
ENDIF()

################################################################################



IF(WIN32 AND NOT ANDROID)

    IF(MSVC)
        # turn off various warnings
        # foreach(warning 4244 4251 4267 4275 4290 4786 4305 4996)
        #     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${warning}")
        # endforeach(warning)

        OPTION(MSVC_DISABLE_CHECKED_ITERATORS "Set to ON to disable Visual C++ checked iterators. If you do this you must ensure that every other project in your solution and all dependencies are compiled with _SECURE_SCL=0." OFF)
        MARK_AS_ADVANCED(MSVC_DISABLE_CHECKED_ITERATORS)
        IF(MSVC_DISABLE_CHECKED_ITERATORS)
            ADD_DEFINITIONS(-D_SECURE_SCL=0)
        ENDIF(MSVC_DISABLE_CHECKED_ITERATORS)

        OPTION(MSVC_USE_DEFAULT_STACK_SIZE "Set to ON to use the default Visual C++ stack size. CMake forces a high stack size by default, which can cause problems for applications with large number of threads." OFF)
        MARK_AS_ADVANCED(MSVC_USE_DEFAULT_STACK_SIZE)
        IF(MSVC_USE_DEFAULT_STACK_SIZE)
            STRING(REGEX REPLACE "/STACK:[0-9]+" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
            STRING(REGEX REPLACE "/STACK:[0-9]+" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
            STRING(REGEX REPLACE "/STACK:[0-9]+" "" CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")
        ENDIF(MSVC_USE_DEFAULT_STACK_SIZE)

    ENDIF()

########################################################################################################
# the following options are MSVC specific,

# the second option disable incremental linking in debug build , that is enabled by default by CMake
##########################################################################################################

    IF(MSVC)
        IF(${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 AND ${CMAKE_PATCH_VERSION} LESS 7)
            MESSAGE("Warning:  disabling versioned options 2.4.6 exibits inconsintencies in .pdb naming, at least under MSVC, suggested upgrading at least to 2.4.7")
            SET(OpenVIZ_MSVC_VERSIONED_DLL OFF)
            SET(OpenVIZ_MSVC_DEBUG_INCREMENTAL_LINK ON)
        ELSE()
            OPTION(OpenVIZ_MSVC_VERSIONED_DLL "Set to ON to build OpenVIZ with versioned dll names" ON)
            MARK_AS_ADVANCED(OpenVIZ_MSVC_VERSIONED_DLL)
            OPTION(OpenVIZ_MSVC_DEBUG_INCREMENTAL_LINK "Set to OFF to build OpenVIZ without incremental linking in debug (release is off by default)" ON)
            MARK_AS_ADVANCED(OpenVIZ_MSVC_DEBUG_INCREMENTAL_LINK)
            IF(NOT OpenVIZ_MSVC_DEBUG_INCREMENTAL_LINK)
                SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
                SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
                SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
            ENDIF(NOT OpenVIZ_MSVC_DEBUG_INCREMENTAL_LINK)
        ENDIF()
    ENDIF(MSVC)
ENDIF(WIN32 AND NOT ANDROID)

# ------------------------------------------------------------------------------
# 加载引入Qt
# ------------------------------------------------------------------------------

# Add external libraries  Qt
INCLUDE(QtSupport)

# ------------------------------------------------------------------------------
# 加载引入Osg
# ------------------------------------------------------------------------------
find_package (osg REQUIRED)

# 自定义宏函数引入
INCLUDE(OpenVizMacroUtils)

# 3rd Party第三方依赖包引入
IF(WIN32 AND NOT ANDROID)
    INCLUDE(Find3rdPartyDependencies)
    INCLUDE(LiblasSupport)
ENDIF()

# glew C++ Libraries support绑定包含目录第三方支持库头文件目录
INCLUDE_DIRECTORIES(
	${ACTUAL_3RDPARTY_DIR}
)

# Boost C++ Libraries support  boost库引入指定1.60版本
INCLUDE(BoostSupport)




####################安装属性配置############################################################
# Installation stuff

SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
SET(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
SET(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")

# Set the build postfix extension according to what configuration is being built.
IF (CMAKE_BUILD_TYPE MATCHES "Release")
    SET(CMAKE_BUILD_POSTFIX "${CMAKE_RELEASE_POSTFIX}")
ELSEIF (CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
    SET(CMAKE_BUILD_POSTFIX "${CMAKE_MINSIZEREL_POSTFIX}")
ELSEIF(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    SET(CMAKE_BUILD_POSTFIX "${CMAKE_RELWITHDEBINFO_POSTFIX}")
ELSEIF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(CMAKE_BUILD_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ELSE()
    SET(CMAKE_BUILD_POSTFIX "")
ENDIF()

#SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME})
SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
MAKE_DIRECTORY(${OUTPUT_BINDIR})
IF(MSVC AND NOT MSVC_IDE)
    MAKE_DIRECTORY(${OUTPUT_BINDIR}/${OSG_PLUGINS})
ENDIF(MSVC AND NOT MSVC_IDE)

#SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib/${CMAKE_SYSTEM_NAME})
SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})
IF(NOT MSVC OR MSVC_IDE)
    MAKE_DIRECTORY(${OUTPUT_LIBDIR}/${OSG_PLUGINS})
ENDIF(NOT MSVC OR MSVC_IDE)

# On CMake 2.4.x use EXECUTABLE_OUTPUT_PATH and LIBRARY_OUTPUT_PATH and later
# we work around the DLL placement by use of the PREFIX target property hack
#
# On CMake 2.6.x use the newly minted CMAKE_LIBRARY_OUTPUT_DIRECTORY,
# CMAKE_ARCHIVE_OUTPUT_DIRECTORY & CMAKE_RUNTIME_OUTPUT_DIRECTORY
#
# CMake >= 2.8.1 changed the output directory algorithm (See doc).
# Here we also set per-configuration directories (CMAKE_*_OUTPUT_DIRECTORY_<CONFIG>), or else binaries are generated in /bin/Debug and /bin/Release, etc. with MSVC and Xcode.
# (Doc reads "multi-configuration generators (VS, Xcode) do NOT append a per-configuration subdirectory to the specified directory").
# The workaround for 2.6.x (adding "../" as an output prefix for each target) seem to have no effect in >=2.8.1, so there is no need to change this.
IF(CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION LESS 5)
    # If CMake < 2.6.0
    SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_BINDIR})
    SET(LIBRARY_OUTPUT_PATH    ${OUTPUT_LIBDIR})
ELSE()
    # If CMake >= 2.6.0
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
    IF(WIN32)
        SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
    ELSE(WIN32)
        SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
    ENDIF(WIN32)

    # Testing CMAKE_VERSION is possible in >= 2.6.4 only
    BUILDER_VERSION_GREATER(2 8 0)
    IF(VALID_BUILDER_VERSION)  # If CMake >= 2.8.1
        FOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})        # For each configuration (Debug, Release, MinSizeRel... and/or anything the user chooses)
            STRING(TOUPPER "${CONF}" CONF)                # Go uppercase (DEBUG, RELEASE...)
            SET("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
            SET("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}")
            IF(WIN32)
                SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}")
            ELSE()
                SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
            ENDIF()
        ENDFOREACH()
    ENDIF(VALID_BUILDER_VERSION)
ENDIF()

# Dynamic vs Static Linking
OPTION(DYNAMIC_OPENVIZ "Set to ON to build OpenSceneGraph for dynamic linking.  Use OFF for static." ON)
IF   (DYNAMIC_OPENVIZ)
    SET(OPENVIZ_USER_DEFINED_DYNAMIC_OR_STATIC "SHARED")
ELSE ()
    SET(OPENVIZ_USER_DEFINED_DYNAMIC_OR_STATIC "STATIC")
ENDIF()

#！添加子目录
ADD_SUBDIRECTORY(src)

OPTION(BUILD_OpenVIZ_TESTS "Build experimental components" ON)
IF(BUILD_OpenVIZ_TESTS)
    # The shader effect compositor in progress
    #ADD_SUBDIRECTORY(test)
ENDIF(BUILD_OpenVIZ_TESTS)

# OPENVIZ Applications
OPTION(BUILD_OpenVIZ_APPLICATIONS "Enable to build OPENVIZ Applications (e.g. OPENVIZ)" ON)
IF(BUILD_OpenVIZ_APPLICATIONS)
    ADD_SUBDIRECTORY(apps)
ENDIF(BUILD_OpenVIZ_APPLICATIONS)

# OPENVIZ Examples
OPTION(BUILD_OpenVIZ_EXAMPLES "Enable to build OPENVIZ Examples" OFF)

# tools
# ADD_SUBDIRECTORY(tools)

