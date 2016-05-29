CXXFLAGS = -std=c++14 -Wall

all : main

main : main.o Map.o Tile.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean :
	rm -f main *.o *~

.PHONY : all clean

