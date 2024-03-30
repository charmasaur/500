#include "chatserver.h"

int main() {
	
	cMyServer server;

	if(server.init("1337")) {
		server.setStdin(true);
		while(!server.getQuit()) server.update();
	}
	return 0;
}
