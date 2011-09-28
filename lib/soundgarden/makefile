OBJECTS = soundgarden.o soundgarden_sdlmixer.o sgplaylist.o

libsoundgarden.a : $(OBJECTS)


include ../standard.mk
include ../includes.mk

CXXFLAGS += $(shell $(INSTALLDIR)/bin/sdl-config --cflags)
CXXFLAGS += -DUSE_SDL
