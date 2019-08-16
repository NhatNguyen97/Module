all: 	run1
run1: 	server.o
	gcc -o run1 server.o
server.o:	server.c
	gcc -c server.c
clean: 
	rm run1 server.o

all: 	run2
run2: 	client.o
	gcc -o run2 client.o -lpthread
client.o:	client.c
	gcc -c client.c -lpthread
clean: 
	rm run2 client.o


