GLUT_OBJECTS = gucci_glut.o resize_unix.o
SDL_OBJECTS = gucci_sdl.o

OBJECTS = gucci.o image.o $(SDL_OBJECTS)

libgucci.a : $(OBJECTS)

include ../standard.mk
include ../includes.mk

CXXFLAGS += -DUSE_SDL -DUSE_FTGL



