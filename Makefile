CXXFLAGS=-Og -ggdb3
CXX=i686-w64-mingw32-g++

ddraw.dll: ddraw.o ddraw.def
	$(CXX) $(CXXFLAGS) -m32 -o ddraw.dll $^ -mdll -static-libgcc -static-libstdc++ -lole32

ddraw.o: ddraw.cpp
	$(CXX) $(CXXFLAGS) -m32 -c -o $@ $^

clean:
	rm -f *.o *.a *.dll
