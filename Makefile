CXXFLAGS = -std=c++14 -Wall

all : main

main : main.cc Map.cc Map.hh MapHelpers.hh
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean :
	rm -f main *.o *~

.PHONY : all clean

