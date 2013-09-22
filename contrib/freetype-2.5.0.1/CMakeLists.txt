cmake_minimum_required(VERSION 2.8)

set(AUTOFIT_SOURCES "src/autofit/autofit.c")
set(BASE_SOURCES
    "src/base/ftbase.c"
    "src/base/ftbbox.c"
    "src/base/ftbdf.c"
    "src/base/ftbitmap.c"
    "src/base/ftdebug.c"
    "src/base/ftgasp.c"
    "src/base/ftglyph.c"
    "src/base/ftgxval.c"
    "src/base/ftinit.c"
    "src/base/ftlcdfil.c"
    "src/base/ftmm.c"
    "src/base/ftotval.c"
    "src/base/ftpfr.c"
    "src/base/ftstroke.c"
    "src/base/ftsynth.c"
    "src/base/ftsystem.c"
    "src/base/fttype1.c"
    "src/base/ftwinfnt.c"
    "src/base/ftxf86.c"
    "src/base/ftpatent.c")
set(BDF_SOURCES "src/bdf/bdf.c")
set(CACHE_SOURCES "src/cache/ftcache.c")
set(CFF_SOURCES "src/cff/cff.c")
set(CID_SOURCES "src/cid/type1cid.c")
set(PCF_SOURCES "src/pcf/pcf.c")
set(BZIP2_SOURCES "src/bzip2/ftbzip2.c")
set(GZIP_SOURCES "src/gzip/ftgzip.c")
set(LZW_SOURCES "src/lzw/ftlzw.c")
set(PFR_SOURCES "src/pfr/pfr.c")
set(PSAUX_SOURCES "src/psaux/psaux.c")
set(PSHINTER_SOURCES "src/pshinter/pshinter.c")
set(PSNAMES_SOURCES "src/psnames/psnames.c")
set(RASTER_SOURCES "src/raster/raster.c")
set(SMOOTH_SOURCES "src/smooth/smooth.c")
set(SFNT_SOURCES "src/sfnt/sfnt.c")
set(TRUETYPE_SOURCES "src/truetype/truetype.c")
set(TYPE1_SOURCES "src/type1/type1.c")
set(TYPE42_SOURCES "src/type42/type42.c")
set(WINFONTS_SOURCES "src/winfonts/winfnt.c")
if(APPLE)
    set(BASE_SOURCES "${BASE_SOURCES}" "src/base/ftmac.c")
endif(APPLE)
set(FREETYPE_SOURCES
    "include/freetype/config/ftheader.h"
    "include/freetype/internal/internal.h"
    ${AUTOFIT_SOURCES}
    ${BASE_SOURCES}
    ${BDF_SOURCES}
    ${CACHE_SOURCES}
    ${CFF_SOURCES}
    ${CID_SOURCES}
    ${PCF_SOURCES}
    ${BZIP2_SOURCES}
    ${GZIP_SOURCES}
    ${LZW_SOURCES}
    ${PFR_SOURCES}
    ${PSAUX_SOURCES}
    ${PSHINTER_SOURCES}
    ${PSNAMES_SOURCES}
    ${RASTER_SOURCES}
    ${SMOOTH_SOURCES}
    ${SFNT_SOURCES}
    ${TRUETYPE_SOURCES}
    ${TYPE1_SOURCES}
    ${TYPE42_SOURCES}
    ${WINFONTS_SOURCES})
add_library(freetype SHARED ${FREETYPE_SOURCES})
target_compile_definitions(freetype PRIVATE -DFT2_BUILD_LIBRARY)
