all: code
.PHONY : clean 
code: code.cpp
	g++ *.cpp  -std=c++11 -o proj
clean:
	rm -rf *.o *.out 
