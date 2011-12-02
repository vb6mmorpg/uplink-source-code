project(uplink)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(UPLINK_BUILD_TYPE ExeOnly CACHE STRING "The build target. Can be Full, Patch, Demo or ExeOnly")

find_package(FTGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(TIFF REQUIRED)

include_directories(contrib)
include_directories(lib/bungle)
include_directories(lib/eclipse)
include_directories(lib/gucci)
include_directories(lib/mmgr)
include_directories(lib/redshirt)
include_directories(lib/soundgarden)
include_directories(lib/tosser)
include_directories(lib/vanbakel)

add_subdirectory(contrib)
add_subdirectory(lib)
add_subdirectory(uplink/src)

if(${UPLINK_BUILD_TYPE} STREQUAL "Full")
    set(UPLINK_PACKAGE_FILES
        Installer/data/changes.txt
        Installer/data/data.dat
        Installer/data/fonts.dat
        Installer/data/graphics.dat
        Installer/data/license.txt
        Installer/data/loading.dat
        Installer/data/mods.txt
        Installer/data/music.dat
        Installer/data/patch.dat
        Installer/data/patch2.dat
        Installer/data/patch3.dat
        Installer/data/readme.txt
        Installer/data/sounds.dat
        Installer/data/uplink.png
        Installer/data/world.dat
    )
elseif(${UPLINK_BUILD_TYPE} STREQUAL "Patch")
    set(UPLINK_PACKAGE_FILES
        Installer/data/changes.txt
        Installer/data/license.txt
        Installer/data/mods.txt
        Installer/data/patch.dat
        Installer/data/patch2.dat
        Installer/data/patch3.dat
        Installer/data/readme.txt
        Installer/data/sounds.dat
        Installer/data/uplink.png
    )
elseif(${UPLINK_BUILD_TYPE} STREQUAL "Demo")
    set(UPLINK_PACKAGE_FILES
        Installer/data/changes.txt
        Installer/data/data.dat
        Installer/data/fonts.dat
        Installer/data/graphics.dat
        Installer/data/license.txt
        Installer/data/loading.dat
        Installer/data/mods.txt
        Installer/data/music.dat
        Installer/data/patch.dat
        Installer/data/patch2.dat
        Installer/data/patch3.dat
        Installer/data/readme.txt
        Installer/data/sounds.dat
        Installer/data/uplink.png
    )
elseif(${UPLINK_BUILD_TYPE} STREQUAL "ExeOnly")
    set(UPLINK_PACKAGE_FILES
        Installer/data/changes.txt
        Installer/data/license.txt
        Installer/data/mods.txt
        Installer/data/readme.txt
        Installer/data/uplink.png
    )
else()
    message(FATAL_ERROR "UPLINK_BUILD_TYPE is set to an invalid value")
endif()


if(UNIX AND ${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(UPLINK_LIB_DEST "lib64")
    set(UPLINK_PLATFORM "amd64")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "uplink-x64")
elseif(UNIX AND ${CMAKE_SIZEOF_VOID_P} EQUAL 4)
    set(UPLINK_LIB_DEST "lib")
    set(UPLINK_PLATFORM "i386")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "uplink-x86")
elseif(WIN32)
    set(UPLINK_LIB_DEST ".")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "uplink")
endif()

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(UNIX)
    get_property(EXESUFFIX TARGET uplink PROPERTY SUFFIX)
    set(CPACK_STRIP_FILES uplink${EXESUFFIX})
    set(CPACK_GENERATOR "STGZ;TGZ")
endif()

file(GLOB UPLINK_PACKAGE_LIBRARIES "${CMAKE_SOURCE_DIR}/instlibs/*")

SET(CMAKE_INSTALL_RPATH "$ORIGIN/${UPLINK_LIB_DEST}")
install(FILES ${UPLINK_PACKAGE_FILES} DESTINATION .)
install(FILES ${UPLNK_PACKAGE_LIBRARIES} DESTINATION ${UPLINK_LIB_DEST})

set(CPACK_VENDOR "Introversion Software")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/Installer/data/license.txt)
set(CPACK_PACKAGE_VERSION_MAJOR "10")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

if(UPLINK_PLATFORM)
    set(CPACK_PACKAGE_FILE_NAME "uplink_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}-${CPACK_PACKAGE_VERSION_PATCH}_${UPLINK_PLATFORM}")
else()
    set(CPACK_PACKAGE_FILE_NAME "uplink_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}-${CPACK_PACKAGE_VERSION_PATCH}")
endif()

include(CPack)