set(LIBNAME "Foundation")
set(POCO_LIBNAME "Poco${LIBNAME}")

# Sources
file(GLOB SRCS_G "src/*.cpp")
POCO_SOURCES_AUTO( SRCS ${SRCS_G})

# Headers
file(GLOB_RECURSE HDRS_G "include/*.h" )
POCO_HEADERS_AUTO( SRCS ${HDRS_G})

# Platform Specific
POCO_SOURCES_AUTO_PLAT( SRCS OPENVMS src/OpcomChannel.cpp )
POCO_HEADERS_AUTO( SRCS include/Poco/OpcomChannel.h )

POCO_SOURCES_AUTO_PLAT( SRCS UNIX src/SyslogChannel.cpp )
POCO_HEADERS_AUTO( SRCS include/Poco/SyslogChannel.h )

# For Windows CE we need to disable these
if(WINCE)
POCO_SOURCES_AUTO_PLAT( SRCS OFF
    src/WindowsConsoleChannel.cpp
    src/EventLogChannel.cpp
    )
else()
POCO_SOURCES_AUTO_PLAT( SRCS WIN32
    src/WindowsConsoleChannel.cpp
    src/EventLogChannel.cpp
    )
endif()

# Messages
POCO_MESSAGES( SRCS Logging src/pocomsg.mc)

# If POCO_UNBUNDLED is enabled we try to find the required packages
# The configuration will fail if the packages are not found
if (POCO_UNBUNDLED)
    find_package(PCRE REQUIRED)
    set(SYSLIBS ${SYSLIBS} ${PCRE_LIBRARIES})
    include_directories(${PCRE_INCLUDE_DIRS})

    #HACK: Unicode.cpp requires functions from these files. The can't be taken from the library
    POCO_SOURCES( SRCS RegExp
        src/pcre_ucd.c
        src/pcre_tables.c
    )

    find_package(ZLIB REQUIRED)
    set(SYSLIBS ${SYSLIBS} ${ZLIB_LIBRARIES})
    include_directories(${ZLIB_INCLUDE_DIRS})
else()
    # pcre
    POCO_SOURCES( SRCS pcre
        src/pcre_config.c
        src/pcre_byte_order.c
        src/pcre_chartables.c
        src/pcre_compile.c
        src/pcre_exec.c
        src/pcre_fullinfo.c
        src/pcre_globals.c
        src/pcre_maketables.c
        src/pcre_newline.c
        src/pcre_ord2utf8.c
        src/pcre_study.c
        src/pcre_tables.c
        src/pcre_dfa_exec.c
        src/pcre_get.c
        src/pcre_jit_compile.c
        src/pcre_refcount.c
        src/pcre_string_utils.c
        src/pcre_version.c
        src/pcre_ucd.c
        src/pcre_valid_utf8.c
        src/pcre_xclass.c
    )

    # zlib
    POCO_HEADERS( SRCS zlib
        include/Poco/zconf.h
        include/Poco/zlib.h
        )

    POCO_SOURCES( SRCS zlib
        src/adler32.c
        src/compress.c
        src/crc32.c
        src/deflate.c
        src/infback.c
        src/inffast.c
        src/inflate.c
        src/inftrees.c
        src/trees.c
        src/zutil.c
    )
endif (POCO_UNBUNDLED)

if(WIN32)
    set(SYSLIBS ${SYSLIBS} iphlpapi)
endif(WIN32)
if (${CMAKE_CXX_COMPILER_ID} MATCHES "SunPro")
    set_target_properties( "${LIBNAME}" PROPERTIES LINK_FLAGS "-library=stlport4")
endif (${CMAKE_CXX_COMPILER_ID} MATCHES "SunPro")

# TODO: Why is this here?
add_definitions( -DPCRE_STATIC)

add_library( "${LIBNAME}" ${LIB_MODE} ${SRCS})
add_library( "${POCO_LIBNAME}" ALIAS "${LIBNAME}")
set_target_properties( "${LIBNAME}"
    PROPERTIES
    VERSION ${PROJECT_VERSION} SOVERSION ${SHARED_LIBRARY_VERSION}
    OUTPUT_NAME ${POCO_LIBNAME}
    DEFINE_SYMBOL Foundation_EXPORTS
    )

target_link_libraries( "${LIBNAME}" ${SYSLIBS})
target_include_directories( "${LIBNAME}"
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

POCO_INSTALL("${LIBNAME}")
POCO_GENERATE_PACKAGE("${LIBNAME}")

if (ENABLE_TESTS)
    add_subdirectory( samples )
    add_subdirectory( testsuite )
endif ()

