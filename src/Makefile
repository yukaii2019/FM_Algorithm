hw2.exe:fm.o main.o
	g++ -O3 fm.o main.o -o ../bin/hw2 
fm.o:fm.cpp fm.h 
	g++ -c fm.cpp
main.o:main.cpp fm.h 
	g++ -c main.cpp
	
clean:
	rm -f fm.o main.o ../bin/hw2
