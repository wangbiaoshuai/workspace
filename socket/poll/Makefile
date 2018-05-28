all: epoll_server

CC = g++
INCLUDE = -I./thread_pool
LIBS = -Wl,-R,"./thread_pool" \
	   -L./thread_pool -lthread_pool
CXXFLAGS = ${INCLUDE} ${LIBS}

epoll_server: epoll_server.cc
	${CC} -o $@ $^ ${CXXFLAGS}

.PHONY: clean

clean:
	rm -rf epoll_server
