client500 : 500/theclient.cpp 500/500global.h 500/500client.o Client/client.o Curses/input.o Curses/output.o
	g++ 500/theclient.cpp 500/500client.o Client/client.o Curses/input.o Curses/output.o -o client500 -lncurses 
500/500client.o : 500/500global.h 500/500client.h 500/500client.cpp constants.h
	g++ 500/500client.cpp -c -o 500/500client.o
Client/client.o : Client/client.cpp Client/client.h constants.h
	g++ Client/client.cpp -c -o Client/client.o
Curses/input.o : Curses/input.cpp Curses/input.h
	g++ Curses/input.cpp -c -o Curses/input.o
Curses/output.o : Curses/output.cpp Curses/output.h
	g++ Curses/output.cpp -c -o Curses/output.o
clean :
	rm client500 500/500client.o Client/client.o Curses/input.o Curses/output.o
