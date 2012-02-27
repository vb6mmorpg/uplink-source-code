export CC = apgcc
export CXX = apg++
RMF = rm -f 
#CXXFLAGS += -g
#CXXFLAGS += -DSLASHER -D_DEBUG
#CXXFLAGS += -I/usr/X11R6/include -D_REENTRANT -fvisibility=default
CXXFLAGS += -D_REENTRANT -fvisibility=default
CXXFLAGS += -O2

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	-$(RMF) *.a *.o *.lib 

%.a: 
	-$(RMF) $@
	$(AR) rvs $@ $+

