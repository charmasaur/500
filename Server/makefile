chat : ../constants.h chat.cpp server.o clientcon.o chatserver.o
	g++ chat.cpp *.o -o chat
chatserver.o : ../constants.h chatserver.h chatserver.cpp 
	g++ chatserver.cpp -c -o chatserver.o
server.o : ../constants.h server.cpp server.h 
	g++ server.cpp -c -o server.o
clientcon.o : ../constants.h clientcon.cpp clientcon.h 
	g++ clientcon.cpp -c -o clientcon.o
clean :
	rm chat *.o
