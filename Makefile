ddraw.dll: ddraw.o ddraw.def
	$(CXX) -m32 -o ddraw.dll $^ -mdll -static-libgcc -static-libstdc++ -lole32

ddraw.o: ddraw.cpp
	$(CXX) -m32 -c -o $@ $^

clean:
	rm -f *.o *.a *.dll
