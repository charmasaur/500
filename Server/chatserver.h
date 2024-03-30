#ifndef NSERVER_H_
#define NSERVER_H_
#include "server.h"

class cMyServer : public cServer {
	private:
		char clientNames[SERVER_MAX_CLIENTS][MAX_NAME_LEN];

		void processNewUser(int user);
		void processNewData(int user);
		void processDisconnectUser(int user);
		bool toquit;
	public:
		cMyServer() {toquit=false; }
		bool getQuit() { return toquit;}
};
#endif
