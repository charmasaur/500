/*
	Handle a connection to a server.
	- Respond to pings from the server
	- Send packets to the server
	- Recieve and process packets
*/

#ifndef CLIENT_H_
#define CLIENT_H_

#include "../constants.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <cstdio>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <errno.h>
using namespace std;

class cClient {
	private:
		FILE*	log;
		int		theSocket;
		fd_set	master;
		bool	checkStdin;

		bool connected;
	protected:
		char	logData[MAX_LOG_ENTRY];
		int		packetSize;	
		char	packet[MAX_PACKET_SIZE];

		void disconnect();

		void sendToServer(char *msg, int len);

		void sendToLog();
		virtual void processNewData(int user);
		virtual void processDisconnect();
	public:
		cClient();
		~cClient();
		void setLog(FILE *logfile);
		void setStdin(bool b);

		bool init(char* ip, char *port);

		virtual void update();
		
		bool getConnected() { return connected; }
};
#endif
