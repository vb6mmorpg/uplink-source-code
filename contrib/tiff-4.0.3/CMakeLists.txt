cmake_minimum_required(VERSION 2.8)

include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckCSourceCompiles)
include(CheckSymbolExists)
include(CheckTypeSize)
include(TestBigEndian)

# Options to toggle various internal codecs.
option(CCITT_SUPPORT
    "Support CCITT Group 3 & 4 algorithms"
    ON)
option(PACKBITS_SUPPORT
    "Support Macintosh PackBits algorithm"
    ON)
option(LZW_SUPPORT
    "Support LZW algorithm"
    ON)
option(THUNDER_SUPPORT
    "Support ThunderScan 4-bit RLE algorithm"
    ON)
option(NEXT_SUPPORT
    "Support NeXT 2-bit RLE algorithm"
    ON)
option(LOGLUV_SUPPORT
    "Support LogLuv high dynamic range encoding"
    ON)
option(MDI_SUPPORT
    "Support Microsoft Document Imaging format"
    ON)
# This is where you would normally check for zlib. We don't need to do that, since it's also built via CMake.
option(PIXARLOG_SUPPORT
    "Support Pixar log-format algorithm (requires Zlib)"
    ON)
# Hmm. Do we need JPEG compression support? Must investigate.
#option(JPEG_SUPPORT
    #"Support JPEG compression (requires IJG JPEG library)"
    #ON)
#option(OJPEG_SUPPORT
    #"Support Old JPEG compresson (read-only)"
    #ON)
#option(JBIG_SUPPORT
    #"Support ISO JBIG compression (requires JBIG-KIT library)"
    #ON)
# Pretty sure we don't need LZMA2 compression.
# We don't need JPEG 12-bit support.
option(CXX_SUPPORT
    "Support C++ stream API (requires C++ compiler)"
    OFF)
# Options for strip chopping support.
option(STRIPCHOP_DEFAULT
    "Support strip chopping (whether or not to convert single-strip uncompressed images to mutiple strips of specified size to reduce memory usage)"
    ON)
set(STRIP_SIZE_DEFAULT
    "8192"
    CACHE STRING
    "Default size of the strip in bytes (when strip chopping enabled)")
option(DEFER_STRILE_LOAD
    "Support deferred strip/tile offset/size loading (experimental)"
    OFF)
option(CHUNKY_STRIP_READ_SUPPORT
    "Support partial strip reading for large strips (experimental)"
    OFF)
option(DEFAULT_EXTRASAMPLE_AS_ALPHA
    "Treat extra sample as alpha (default enabled). The RGBA interface will treat a fourth sample with no EXTRASAMPLE_ value as being ASSOCALPHA. Many packages produce RGBA files but don't mark the alpha properly."
    ON)
option(CHECK_JPEG_YCBCR_SUBSAMPLING
    "Pick up YCbCr subsampling info from the JPEG data stream to support files lacking the tag (default enabled)."
    ON)
# Check for libm.
CHECK_LIBRARY_EXISTS(m sin "" HAVE_LIBM)
# Check for various header files.
CHECK_INCLUDE_FILE("assert.h" HAVE_ASSERT_H)
CHECK_INCLUDE_FILE("dlfcn.h" HAVE_DLFCN_H)
CHECK_INCLUDE_FILE("fcntl.h" HAVE_FCNTL_H)
CHECK_INCLUDE_FILE("io.h" HAVE_IO_H)
CHECK_INCLUDE_FILE("limits.h" HAVE_LIMITS_H)
CHECK_INCLUDE_FILE("malloc.h" HAVE_MALLOC_H)
CHECK_INCLUDE_FILE("search.h" HAVE_SEARCH_H)
CHECK_INCLUDE_FILE("sys/time.h" HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
# Check for const keyword.
CHECK_C_SOURCE_COMPILES("int main() { const int foo = 5; return 0; }" HAVE_CONST)
if(HAVE_CONST)
    set(TIFF_CONST "/* #undef const */")
else(HAVE_CONST)
    set(TIFF_CONST "#define const")
endif(HAVE_CONST)
# Check for inline keyword.
CHECK_C_SOURCE_COMPILES("inline int foo() { int i = 5; return i; } int main() { return foo(); }" HAVE_INLINE)
CHECK_C_SOURCE_COMPILES("__inline__ int foo() { int i = 5; return i; } int main() { return foo(); }" HAVE___INLINE__)
CHECK_C_SOURCE_COMPILES("__inline int foo() { int i = 5; return i; } int main() { return foo(); }" HAVE___INLINE)
if(HAVE_INLINE)
    set(TIFF_INLINE "/* #undef inline */")
elseif(HAVE___INLINE__)
    set(TIFF_INLINE "#define inline __inline__")
elseif(HAVE___INLINE)
    set(TIFF_INLINE "#define inline __inline")
else(HAVE_INLINE)
    set(TIFF_INLINE "#define inline")
endif(HAVE_INLINE)
# Check target endianess.
TEST_BIG_ENDIAN(HOST_BIGENDIAN)
if(HOST_BIGENDIAN)
    set(HOST_FILLORDER "FILLORDER_MSB2LSB")
else(HOST_BIGENDIAN)
    set(HOST_FILLORDER "FILLORDER_LSB2MSB")
endif(HOST_BIGENDIAN)
# Check for off_t.
CHECK_TYPE_SIZE("off_t" OFF_T)
if(NOT HAVE_OFF_T)
    set(off_t "long int")
endif(NOT HAVE_OFF_T)
# Check for size_t.
CHECK_TYPE_SIZE("size_t" SIZE_T)
if(NOT HAVE_SIZE_T)
    set(size_t "unsigned int")
endif(NOT HAVE_SIZE_T)
# Check if <time.h> and <sys/time.h> can both be included by the same translation unit.
CHECK_C_SOURCE_COMPILES("#include <sys/time.h> #include <time.h> int main() { return 0; }" TIME_WITH_SYS_TIME)
# Check for struct tm in <time.h>.
CHECK_SYMBOL_EXISTS("struct tm" "time.h" TM_IN_SYS_TIME)
# Check built-in type sizes.
CHECK_TYPE_SIZE("signed short" SIZEOF_SIGNED_SHORT)
CHECK_TYPE_SIZE("unsigned short" SIZEOF_UNSIGNED_SHORT)
CHECK_TYPE_SIZE("signed int" SIZEOF_SIGNED_INT)
CHECK_TYPE_SIZE("unsigned int" SIZEOF_UNSIGNED_INT)
CHECK_TYPE_SIZE("signed long" SIZEOF_SIGNED_LONG)
CHECK_TYPE_SIZE("unsigned long" SIZEOF_UNSIGNED_LONG)
CHECK_TYPE_SIZE("signed long long" SIZEOF_SIGNED_LONG_LONG)
CHECK_TYPE_SIZE("unsigned long long" SIZEOF_UNSIGNED_LONG_LONG)
CHECK_TYPE_SIZE("unsigned char *" SIZEOF_UNSIGNED_CHAR_P)
# Define TIFF basic data types.
#if(HAVE_STDINT_H)
    #set(TIFF_INT8_T "int8_t")
    #set(TIFF_UINT8_T "uint8_t")
    #set(TIFF_INT16_T "int16_t")
    #set(TIFF_UINT16_T "uint16_t")
    #set(TIFF_INT32_T "int32_t")
    #set(TIFF_UINT32_T "uint32_t")
    #set(TIFF_INT64_T "int64_t")
    #set(TIFF_UINT64_T "uint64_t")
    #set(TIFF_INT32_FORMAT "\"%ld\"")
    #set(TIFF_UINT32_FORMAT "\"%lu\"")
    #if(WIN32)
        #set(TIFF_INT64_FORMAT "\"%I64d\"")
        #set(TIFF_UINT64_FORMAT "\"%I64u\"")
    #else(WIN32)
        #set(TIFF_INT64_FORMAT "\"%lld\"")
        #set(TIFF_UINT64_FORMAT "\"%llu\"")
    #endif(WIN32)
#else(HAVE_STDINT_H)
    set(TIFF_INT8_T "signed char")
    set(TIFF_UINT8_T "unsigned char")
    set(TIFF_INT16_T "signed short")
    set(TIFF_UINT16_T "unsigned short")
    if(NOT (TIFF_SIZEOF_UNSIGNED_INT LESS 4))
        set(TIFF_INT32_T "signed int")
        set(TIFF_UINT32_T "unsigned int")
        set(TIFF_INT32_FORMAT "\"%d\"")
        set(TIFF_UINT32_FORMAT "\"%u\"")
    else(NOT (TIFF_SIZEOF_UNSIGNED_INT LESS 4))
        set(TIFF_INT32_T "signed long")
        set(TIFF_UINT32_T "unsigned long")
        set(TIFF_INT32_FORMAT "\"%ld\"")
        set(TIFF_UINT32_FORMAT "\"%lu\"")
    endif(NOT (TIFF_SIZEOF_UNSIGNED_INT LESS 4))
    if(NOT (TIFF_SIZEOF_UNSIGNED_LONG LESS 8))
        set(TIFF_INT64_T "signed long")
        set(TIFF_UINT64_T "unsigned long")
        set(TIFF_INT64_FORMAT "\"%ld\"")
        set(TIFF_INT64_FORMAT "\"%ld\"")
    else(NOT (TIFF_SIZEOF_UNSIGNED_LONG LESS 8))
        set(TIFF_INT64_T "signed long long")
        set(TIFF_UINT64_T "unsigned long long")
        if(WIN32)
            set(TIFF_INT64_FORMAT "\"%I64d\"")
            set(TIFF_UINT64_FORMAT "\"%I64u\"")
        else(WIN32)
            set(TIFF_INT64_FORMAT "\"%lld\"")
            set(TIFF_UINT64_FORMAT "\"%llu\"")
        endif(WIN32)
    endif(NOT (TIFF_SIZEOF_UNSIGNED_LONG LESS 8))
#endif(HAVE_STDINT_H)
# Define TIFF_SSIZE_T.
if(NOT (TIFF_SIZEOF_SIGNED_LONG LESS TIFF_SIZEOF_UNSIGNED_CHAR_P))
    set(TIFF_SSIZE_T "signed long")
    set(TIFF_SSIZE_FORMAT "\"%ld\"")
elseif(NOT (TIFF_SIZEOF_SIGNED_LONG_LONG LESS TIFF_SIZEOF_UNSIGNED_CHAR_P))
    set(TIFF_SSIZE_T "signed long long")
    if(WIN32)
        set(TIFF_SSIZE_FORMAT "\"%I64d\"")
    else(WIN32)
        set(TIFF_SSIZE_FORMAT "\"%lld\"")
    endif(WIN32)
else(NOT (TIFF_SIZEOF_SIGNED_LONG LESS TIFF_SIZEOF_UNSIGNED_CHAR_P))
    set(TIFF_SSIZE_T "unknown")
    set(TIFF_SSIZE_FORMAT "unknown")
endif(NOT (TIFF_SIZEOF_SIGNED_LONG LESS TIFF_SIZEOF_UNSIGNED_CHAR_P))
# Define TIFF_PTRDIFF_T.
CHECK_TYPE_SIZE("ptrdiff_t" PTRDIFF_T)
if(HAVE_PTRDIFF_T)
    set(TIFF_PTRDIFF_T "ptrdiff_t")
    set(TIFF_PTRDIFF_FORMAT "\"%ld\"")
else(HAVE_PTRDIFF_T)
    set(TIFF_PTRDIFF_T ${TIFF_SSIZE_T})
    set(TIFF_PTRDIFF_FORMAT ${SSIZE_FORMAT})
endif(HAVE_PTRDIFF_T)
# Check for int8, int16, int32 types.
CHECK_INCLUDE_FILE("inttypes.h" HAVE_INTTYPES_H)
CHECK_TYPE_SIZE("int8" INT8)
CHECK_TYPE_SIZE("int16" INT16)
CHECK_TYPE_SIZE("int32" INT32)
# Check for various functions.
CHECK_FUNCTION_EXISTS(floor HAVE_FLOOR)
CHECK_FUNCTION_EXISTS(isascii HAVE_ISASCII)
CHECK_FUNCTION_EXISTS(memmove HAVE_MEMMOVE)
CHECK_FUNCTION_EXISTS(memset HAVE_MEMSET)
CHECK_FUNCTION_EXISTS(mmap HAVE_MMAP)
CHECK_FUNCTION_EXISTS(pow HAVE_POW)
CHECK_FUNCTION_EXISTS(setmode HAVE_SETMODE)
CHECK_FUNCTION_EXISTS(sqrt HAVE_SQRT)
CHECK_FUNCTION_EXISTS(strchr HAVE_STRCHR)
CHECK_FUNCTION_EXISTS(strrchr HAVE_STRRCHR)
CHECK_FUNCTION_EXISTS(strstr HAVE_STRSTR)
CHECK_FUNCTION_EXISTS(strtol HAVE_STRTOL)
# These functions have replacements if we can't find them elsewhere.
CHECK_FUNCTION_EXISTS(getopt HAVE_GETOPT)
CHECK_FUNCTION_EXISTS(strcasecmp HAVE_STRCASECMP)
CHECK_FUNCTION_EXISTS(strtoul HAVE_STRTOUL)
CHECK_FUNCTION_EXISTS(strtoull HAVE_STRTOULL)
CHECK_FUNCTION_EXISTS(lfind HAVE_LFIND)
# Hey, don't blame me, this is how it was in configure.ac.
set(HAVE_IEEEFP 1)
# TODO: enable large file support
# Check for OpenGL and GLUT.
#CHECK_INCLUDE_FILE("GLUT/glut.h" HAVE_GLUT_GLUT_H)
#CHECK_INCLUDE_FILE("GL/glut.h" HAVE_GL_GLUT_H)
#CHECK_INCLUDE_FILE("GL/glu.h" HAVE_GL_GLU_H)
#CHECK_INCLUDE_FILE("GL/gl.h" HAVE_GL_GL_H)
# Check for Windows IO.
if(WIN32)
    CHECK_INCLUDE_FILE("windows.h" HAVE_WINDOWS_H)
    if(HAVE_WINDOWS_H)
        set(USE_WIN32_FILEIO ON)
    endif(HAVE_WINDOWS_H)
endif(WIN32)
# Enable subifd support.
set(SUBIFD_SUPPORT ON)
# Some jbig thing. Why do we care?
if(JBIG_SUPPORT)
    CHECK_FUNCTION_EXISTS(jbg_newlen HAVE_JBG_NEWLEN)
endif(JBIG_SUPPORT)
# Finally! Time to set up the sources and move on to more exciting things.
set(PORT_SOURCES "port/libport.h")
if(NOT HAVE_GETOPT)
    set(PORT_SOURCES ${PORT_SOURCES} "port/getopt.c")
endif(NOT HAVE_GETOPT)
if(NOT HAVE_STRCASECMP)
    set(PORT_SOURCES ${PORT_SOURCES} "port/strcasecmp.c")
endif(NOT HAVE_STRCASECMP)
if(NOT HAVE_STRTOUL)
    set(PORT_SOURCES ${PORT_SOURCES} "port/strtoul.c")
endif(NOT HAVE_STRTOUL)
if(NOT HAVE_STRTOULL)
    set(PORT_SOURCES ${PORT_SOURCES} "port/strtoull.c")
endif(NOT HAVE_STRTOULL)
if(NOT HAVE_LFIND)
    set(PORT_SOURCES ${PORT_SOURCES} "port/lfind.c")
endif(NOT HAVE_LFIND)

set(LIBTIFF_SOURCES
    "libtiff/tiff.h"
    "libtiff/tiffio.h"
    "libtiff/tiffvers.h"
    "libtiff/tif_aux.c"
    "libtiff/tif_close.c"
    "libtiff/tif_codec.c"
    "libtiff/tif_color.c"
    "libtiff/tif_compress.c"
    "libtiff/tif_dir.c"
    "libtiff/tif_dirinfo.c"
    "libtiff/tif_dirread.c"
    "libtiff/tif_dirwrite.c"
    "libtiff/tif_dumpmode.c"
    "libtiff/tif_error.c"
    "libtiff/tif_extension.c"
    "libtiff/tif_fax3.c"
    "libtiff/tif_fax3sm.c"
    "libtiff/tif_flush.c"
    "libtiff/tif_getimage.c"
    "libtiff/tif_jbig.c"
    "libtiff/tif_jpeg.c"
    "libtiff/tif_jpeg_12.c"
    "libtiff/tif_luv.c"
    "libtiff/tif_lzma.c"
    "libtiff/tif_lzw.c"
    "libtiff/tif_next.c"
    "libtiff/tif_ojpeg.c"
    "libtiff/tif_open.c"
    "libtiff/tif_packbits.c"
    "libtiff/tif_pixarlog.c"
    "libtiff/tif_predict.c"
    "libtiff/tif_print.c"
    "libtiff/tif_read.c"
    "libtiff/tif_strip.c"
    "libtiff/tif_swab.c"
    "libtiff/tif_thunder.c"
    "libtiff/tif_tile.c"
    "libtiff/tif_version.c"
    "libtiff/tif_warning.c"
    "libtiff/tif_write.c"
    "libtiff/tif_zip.c")
if(WIN32)
    set(TIFF_SYMBOLS "libtiff/tif_win32.c" "libtiff/libtiff.def")
elseif(UNIX)
    set(TIFF_SYMBOLS "libtiff/tif_unix.c" "libtiff/libtiff.map")
endif(WIN32)
set(TIFF_SOURCES ${PORT_SOURCES} ${LIBTIFF_SOURCES} ${TIFF_SYMBOLS})

add_library(tiff SHARED ${TIFF_SOURCES})
target_link_libraries(tiff zlib)
configure_file("libtiff/tif_config.h.cmake.in" "libtiff/tif_config.h")
configure_file("libtiff/tif_config.h.cmake.in" "libtiff/tiffconf.h")
