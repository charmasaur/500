theclient : theclient.cpp 500global.h 500client.o ../Client/client.o ../Curses/input.o ../Curses/output.o
	g++ theclient.cpp 500client.o ../Client/client.o ../Curses/input.o ../Curses/output.o -o theclient -lncurses 
500client.o : 500global.h 500client.h 500client.cpp
	g++ 500client.cpp -c -o 500client.o
theserver : ../constants.h theserver.cpp 500global.h 500server.o ../Server/server.o ../Server/clientcon.o
	g++ theserver.cpp 500server.o ../Server/server.o ../Server/clientcon.o -o theserver 
500server.o : ../constants.h 500global.h 500server.h 500server.cpp 
	g++ 500server.cpp -c -o 500server.o
clean :
	rm theserver theclient *.o
