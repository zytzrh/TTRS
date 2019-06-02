





code: code.o
	g++ -O2 -o code code.o  -std=c++11



code.o: front_end/code.cpp
	g++  -c front_end/code.cpp -std=c++11



clean:
	rm code *.o data/*.txt
