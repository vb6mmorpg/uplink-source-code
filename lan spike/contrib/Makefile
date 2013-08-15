prefix:=$(PWD)/install

#FREETYPE_DIR=freetype-1.3.1
FREETYPE_DIR=freetype-current
GLTT_DIR=gltt-2.5.2
TCP4U_DIR=tcp4u.331
IRCLIB_LINUX_DIR=irclib
SDL_DIR=SDL-1.2.11_dev
SDL_MIXER_DIR=SDL_mixer-1.2.7_patched
TIFF_DIR=tiff-3.7.2
JPEG_DIR=libjpeg6b-6b
ZLIB_DIR=zlib-1.2.2
MIKMOD_DIR=libmikmod
LOKI_SETUPDB_DIR=loki_setupdb
LOKI_SETUP_DIR=loki_setup
UNRAR_DIR=unrar
FREETYPE2_DIR=freetype-2.2.1
FTGL_DIR=FTGL-2.1.2/unix

X11_INC_DIR=/usr/include/X11
X11_LIB_DIR=/usr/lib/X11

export CC=apgcc
export CXX=apg++

all: 
	$(MAKE) config
	$(MAKE) build

config: 
#	$(MAKE) freetype 
#	$(MAKE) gltt-config
	$(MAKE) freetype2
	$(MAKE) ftgl-config
	$(MAKE) sdl
#	$(MAKE) libmikmod-config
	$(MAKE) sdl-mixer-config
	$(MAKE) tiff-config
#	$(MAKE) jpeg-config
#	$(MAKE) zlib-config
	$(MAKE) loki-setup-db
	$(MAKE) loki-setup-config

build:
#	$(MAKE) gltt-build
	$(MAKE) ftgl-build
	$(MAKE) tcp4u-build
	$(MAKE) irclib-linux-build
	$(MAKE) unrar-build
#	$(MAKE) libmikmod-build
	$(MAKE) sdl-mixer-build
	$(MAKE) tiff-build
#	$(MAKE) jpeg-build
#	$(MAKE) zlib-build
	$(MAKE) loki-setup-build

clean:
#	$(MAKE) -C $(FREETYPE_DIR) clean
#	$(MAKE) -C $(GLTT_DIR) clean
	$(MAKE) -C $(FREETYPE2_DIR) clean
	$(MAKE) -C $(FTGL_DIR) clean
	$(MAKE) -C $(SDL_DIR) clean
#	$(MAKE) -C $(MIKMOD_DIR) clean
	$(MAKE) -C $(SDL_MIXER_DIR) clean
	$(MAKE) -C $(TIFF_DIR) clean
#	$(MAKE) -C $(JPEG_DIR) clean
#	$(MAKE) -C $(ZLIB_DIR) clean
	$(MAKE) -C $(LOKI_SETUP_DIR) clean
	$(MAKE) -C $(LOKI_SETUPDB_DIR) clean
	$(MAKE) -C $(IRCLIB_LINUX_DIR) clean
	$(MAKE) -C $(UNRAR_DIR) clean
	cd $(TCP4U_DIR)/src ; make -f ../Unix/Makefile clean

loki-setup-db:
	$(MAKE) loki-setup-db-config
	$(MAKE) loki-setup-db-build

loki-setup-db-config:
	cd $(LOKI_SETUPDB_DIR) ; \
	./autogen.sh ; \
	./configure

loki-setup-db-build:
	$(MAKE) -C $(LOKI_SETUPDB_DIR)

loki-setup-config:
	cd $(LOKI_SETUP_DIR) ; \
	./autogen.sh ; \
	./configure LDFLAGS=-Llibs
#	./configure LDFLAGS="-Llibs -L/usr/lib/gtk-2.0"

loki-setup-build:
	$(MAKE) -C $(LOKI_SETUP_DIR)

tiff-config:
	cd $(TIFF_DIR) ; \
	rm -f config.cache ; \
	./configure --prefix=$(prefix) --enable-static=yes --enable-shared=no --x-includes=$(X11_INC_DIR) --x-libraries=$(X11_LIB_DIR)

tiff-build:
	$(MAKE) -C $(TIFF_DIR)
	$(MAKE) -C $(TIFF_DIR) install

libmikmod-config:
	cd $(MIKMOD_DIR) ; \
	rm -f config.cache ; \
	./configure --prefix=$(prefix)

libmikmod-build:
	$(MAKE) -C $(MIKMOD_DIR)
	$(MAKE) -C $(MIKMOD_DIR) install


zlib-config:
	cd $(ZLIB_DIR) ; \
	rm -f config.cache ; \
	./configure --prefix=$(prefix)

zlib-build:
	$(MAKE) -C $(ZLIB_DIR)
	$(MAKE) -C $(ZLIB_DIR) install

jpeg-config:
	cd $(JPEG_DIR) ; \
	rm -f config.cache ; \
	./configure --prefix=$(prefix)  --enable-shared=no

jpeg-build:
	$(MAKE) -C $(JPEG_DIR)
	$(MAKE) -C $(JPEG_DIR) install
	$(MAKE) -C $(JPEG_DIR) install-lib

freetype:
	$(MAKE) freetype-config
	$(MAKE) freetype-build

freetype-config:
	cd $(FREETYPE_DIR) ; \
	rm -f config.cache ; \
	./configure --prefix=$(prefix) --enable-static=yes --enable-shared=no --x-includes=$(X11_INC_DIR) --x-libraries=$(X11_LIB_DIR)

freetype-build:
	$(MAKE) -C $(FREETYPE_DIR) 
	$(MAKE) -C $(FREETYPE_DIR) install

gltt-config: 
	cd $(GLTT_DIR) ; \
	rm -f config.cache ; \
	./configure --prefix=$(prefix) --with-ttf-dir=$(prefix) --x-includes=$(X11_INC_DIR) --x-libraries=$(X11_LIB_DIR)

gltt-build:
	$(MAKE) -C $(GLTT_DIR) 
	$(MAKE) -C $(GLTT_DIR) install

freetype2:
	$(MAKE) freetype2-config
	$(MAKE) freetype2-build

freetype2-config:
	cd $(FREETYPE2_DIR) ; \
	rm -f config.cache ; \
	export PATH=$(prefix)/bin:$(PATH) ; \
	export LD_LIBRARY_PATH=$(prefix)/lib:$(LD_LIBRARY_PATH) ; \
	./autogen.sh ; \
	./configure --prefix=$(prefix) --enable-static=no --enable-shared=yes --x-includes=$(X11_INC_DIR) --x-libraries=$(X11_LIB_DIR)
#	./configure --prefix=$(prefix) --enable-static=yes --enable-shared=no --x-includes=$(X11_INC_DIR) --x-libraries=$(X11_LIB_DIR)

freetype2-build:
	$(MAKE) -C $(FREETYPE2_DIR) 
	$(MAKE) -C $(FREETYPE2_DIR) install

ftgl-config: 
	cd $(FTGL_DIR) ; \
	rm -f config.cache ; \
	export FT2_CONFIG=$(prefix)/bin/freetype-config ; \
	./configure --prefix=$(prefix) --enable-static=no --enable-shared=yes --with-gl-inc=/usr/include --with-gl-lib=/usr/lib --with-ft-prefix=$(prefix) --with-ft-exec-prefix=$(prefix) --x-includes=$(X11_INC_DIR) --x-libraries=$(X11_LIB_DIR)
#	./configure --prefix=$(prefix) --with-gl-inc=/usr/include --with-gl-lib=/usr/lib --with-ft-prefix=$(prefix) --with-ft-exec-prefix=$(prefix) --x-includes=$(X11_INC_DIR) --x-libraries=$(X11_LIB_DIR)

ftgl-build:
	$(MAKE) -C $(FTGL_DIR) 
	$(MAKE) -C $(FTGL_DIR) install

sdl:
	$(MAKE) sdl-config
	$(MAKE) sdl-build

sdl-config:
	cd $(SDL_DIR) ; \
	./configure --prefix=$(prefix) --x-includes=$(X11_INC_DIR) --x-libraries=$(X11_LIB_DIR)

sdl-build:
	$(MAKE) -C $(SDL_DIR) 
	$(MAKE) -C $(SDL_DIR) install

sdl-mixer-config:
	cd $(SDL_MIXER_DIR) ; \
	rm -f config.cache ; \
	export PATH=$(prefix)/bin:$(PATH) ; \
	export LD_LIBRARY_PATH=$(prefix)/lib:$(LD_LIBRARY_PATH) ; \
	./autogen.sh ; \
	./configure --prefix=$(prefix) --disable-music-mp3 --disable-music-ogg --disable-music-midi
#	./configure --prefix=$(prefix) --disable-music-mp3 --disable-music-ogg --disable-music-midi --enable-music-libmikmod

sdl-mixer-build:
	$(MAKE) -C $(SDL_MIXER_DIR) 
	$(MAKE) -C $(SDL_MIXER_DIR) install

tcp4u-build:
	$(MAKE) -C $(TCP4U_DIR)/src -f ../Unix/Makefile

irclib-linux-build:
	$(MAKE) -C $(IRCLIB_LINUX_DIR)

unrar-build:
	$(MAKE) -C $(UNRAR_DIR)

