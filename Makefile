CXX=g++
CXXFLAGS=-g
LDFLAGS=-lz

all: gzextract

gzextract: main.o gzip.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) main.o gzip.o -o gzextract

main.o: main.cpp gzip.h
	$(CXX) $(CXXFLAGS) -c main.cpp

gzip.o: gzip.cpp gzip.h
	$(CXX) $(CXXFLAGS) -c gzip.cpp

clean:
	rm gzextract *.o
