SOURCES = irc.cpp socket.cpp CrossThreadsMessagingDevice.cpp \
	  linux/windows.cpp linux/missing.cpp linux/winsock.cpp
OBJECTS = $(SOURCES:%.cpp=%.o)
HEADERS = $(shell find . -name '*.h')
CXXFLAGS = -O2 -Wall -Ilinux -g

LIBDIR = ../../lib
INCLUDES += -I$(LIBDIR)/mmgr 
CXXFLAGS += $(INCLUDES)

AR = ar
RMF = rm -f

libirclib.a: $(OBJECTS)
	$(AR) r $@ $(OBJECTS)

clean:
	$(RMF) libirclib.a $(OBJECTS)

all: irclib.a

$(OBJECTS) : $(HEADERS)

test: test.o $(OBJECTS) $(HEADERS)
	c++ -o test test.o $(OBJECTS) -lpthread

