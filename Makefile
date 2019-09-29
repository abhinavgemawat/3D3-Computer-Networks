CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -I -Wall -pthread -std=c++11 $(CXXOPTIMIZE)
USERID=Cian_Mac_Donncha_16316406
CLASSES=server-methods.h server-methods.cpp http-abstract.h http-abstract.cpp  

all: web-client web-server

web-server: $(CLASSES) 
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp

web-client: $(CLASSES) 
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp


clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM web-server web-client *.tar.gz

tarball: clean
	tar -cvf $(USERID).tar.gz *
