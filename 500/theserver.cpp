#include "500server.h"
#include "500global.h"
#include <iostream>
#include <ctime>
using namespace std;

int main() {
	srand(time(NULL));
	c500Server server;
	if(server.init("1500")) {
		server.setStdin(true);
		while(server.getState()!=QUITTING) server.update();
	}
	return 0;
}
