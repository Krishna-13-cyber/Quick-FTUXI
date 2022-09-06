CXX=g++
CXXFLAGS=-W -Wall -pedantic -std=c++14

PROGRAMS= \
    test \

all: $(PROGRAMS)

clean:
	rm -f *.o $(PROGRAMS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test: test.o
	$(CXX) $(CXXFLAGS) -o $@ $^ 