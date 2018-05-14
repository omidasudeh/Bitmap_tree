all: code
.PHONY : clean 
code: code.cpp
	g++ *.cpp  -std=c++11 -o proj -fopenmp -lrt -O3 
clean:
	rm -rf *.o *.out 
