CC=gcc
CFLAGS=-Wall -g -std=c11 -pedantic
LDLIBS=-lrt -pthread  #linux



ALL =  test

all : $(ALL)


test : test.o m_init_mutex.o m_prefix_slash.o m_file.o m_thread_error.o
test.o : test.c
m_init_mutex.o : m_init_mutex.c m_init_mutex.h
m_prefix_slash.o: m_prefix_slash.c m_prefix_slash.h
m_file.o : m_file.c m_file.h
m_thread_error.o : m_thread_error.c m_thread_error.h


clean:
	rm -rf *~
cleanall:
	rm -rf *~ $(ALL) *.o
