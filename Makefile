CPPFLAGS = -std=c++11 -Wno-write-strings
DEBUGFLAGS = -ggdb
CC = g++
.PHONY: clean

all: al.out
	rm -f *.o

al.out: main.o list.o hashTable.o stack.o
	$(CC) $(CPPFLAGS) $(DEBUGFLAGS) -pthread main.o list.o hashTable.o stack.o -o a.out

main.o: main.c
	$(CC) $(CPPFLAGS) $(DEBUGFLAGS) -c main.c

list.o: list.c
	$(CC) $(CPPFLAGS) $(DEBUGFLAGS) -pthread -c list.c

hastTable.o: hastTable.c
	$(CC) $(CPPFLAGS) $(DEBUGFLAGS) -pthread -c hashTable.c

stack.o: stack.c
	$(CC) $(CPPFLAGS) $(DEBUGFLAGS) -pthread -c stack.c

clean:
	rm -f *.out *.o
