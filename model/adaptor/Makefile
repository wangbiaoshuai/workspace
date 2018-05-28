all: main

CXX = g++
main: main.o fast_scan.o
	${CXX} -o $@ $^

main.o: main.cc

fast_scan.o: fast_scan.cc

.PHONY: clean

clean:
	${RM} -f *.o main
