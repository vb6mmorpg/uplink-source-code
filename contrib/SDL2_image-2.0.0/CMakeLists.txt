cmake_minimum_required(VERSION 2.8)

set(SDL_IMAGE_SOURCES
    "miniz.h"
    "SDL_image.h"
    "IMG.c"
    "IMG_bmp.c"
    "IMG_gif.c"
    "IMG_jpg.c"
    "IMG_lbm.c"
    "IMG_pcx.c"
    "IMG_png.c"
    "IMG_pnm.c"
    "IMG_tga.c"
    "IMG_tif.c"
    "IMG_webp.c"
    "IMG_xcf.c"
    "IMG_xpm.c"
    "IMG_xv.c"
    "IMG_xxx.c")
if(WIN32)
    set(SDL_IMAGE_RC "version.rc")
endif(WIN32)

add_library(SDL2_image SHARED ${SDL_IMAGE_SOURCES} ${SDL_IMAGE_RC})
target_compile_definitions(SDL2_image PUBLIC -DLOAD_TIF)
target_link_libraries(SDL2_image SDL2main SDL2 tiff)
