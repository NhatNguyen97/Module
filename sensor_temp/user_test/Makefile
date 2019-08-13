all: 	run
run: 	user_app.o
	gcc -o run user_app.o
user_app.o:	user_app.c
	gcc -c user_app.c
clean: 
	rm run user_app.o
