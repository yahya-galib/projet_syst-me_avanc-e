CC=gcc
CFLAGS=-Wall -g -std=c11 -pedantic
LDLIBS=-lrt -pthread  #linux



ALL =  test test2

all : $(ALL)


m_init_mutex.o : m_init_mutex.c m_init_mutex.h
m_prefix_slash.o: m_prefix_slash.c m_prefix_slash.h
m_thread_error.o : m_thread_error.c m_thread_error.h
m_file.o : m_file.c m_file.h


test : test.c m_init_mutex.o m_prefix_slash.o m_file.o m_thread_error.o
test2 : test2.c m_init_mutex.o m_prefix_slash.o m_file.o m_thread_error.o
test3 : test3.c m_init_mutex.o m_prefix_slash.o m_file.o m_thread_error.o
test4 : test4.c m_init_mutex.o m_prefix_slash.o m_file.o m_thread_error.o

clean:
	rm -rf *~
cleanall:
	rm -rf *~ $(ALL) *.o
