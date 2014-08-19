SOURCES = sha1.cpp
OBJECTS = $(SOURCES:%.cpp=%.o)
HEADERS = $(shell find . -name '*.h')
CXXFLAGS = -O2 -Wall -g

AR = ar
RMF = rm -f

libunrar.a: $(OBJECTS)
	$(AR) r $@ $(OBJECTS)

clean:
	$(RMF) libunrar.a $(OBJECTS)

all: unrar.a

$(OBJECTS) : $(HEADERS)
