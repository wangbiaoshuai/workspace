all: tcp_server

CC = g++

INCLUDE = -I./thread_pool

LIBS = -Wl,-R,"./thread_pool" \
	   -L./thread_pool -lthread_pool

CXXFLAGS = -g ${INCLUDE} ${LIBS}

tcp_server: tcp_server.cc
	${CC} -o $@ $^ ${CXXFLAGS}
