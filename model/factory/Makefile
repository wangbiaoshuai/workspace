all: main

CC = g++

main: main.o factory.o product.o
	${CC} -o $@ $^

main.o: main.cc

factory.o: factory.cc

product.o: product.cc


.PHONY: clean

clean:
	${RM} -f *.o main
