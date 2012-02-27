LIBS=tosser gucci eclipse bungle redshirt slasher mmgr
#CONTRIBDIR = ../../../contrib
CONTRIBDIR = ../../contrib
INSTALLDIR = $(CONTRIBDIR)/install
INCLUDES += $(LIBS:%=-I../%) 
INCLUDES += -I$(INSTALLDIR)/include 
#INCLUDES += -I$(INSTALLDIR)/include/gltt

INCLUDES += -I$(CONTRIBDIR)/install/include/freetype2
INCLUDES += -I$(CONTRIBDIR)/install/include/freetype2/freetype
#INCLUDES += -I$(CONTRIBDIR)/ftgl_dist/src
INCLUDES += -I$(CONTRIBDIR)
CXXFLAGS += $(INCLUDES)
CXXFLAGS += -g
