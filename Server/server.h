/*
	A basic server maintains several stream connections to clients. It also does the following:
	- Pings clients at regular intervals
	- Recieves and processes packets
	- Sends packets
	- Handles requests for new clients
*/
#ifndef SERVER_H_
#define SERVER_H_

#include "../constants.h"
#include <vector>
#include <set>
#define SERVER_MAX_CLIENTS 16
#define SERVER_MAX_LOG_ENTRY MAX_LOG_ENTRY
#define SERVER_MAX_PACKET_SIZE MAX_PACKET_SIZE
#define SERVER_BACKLOG 16
#include <errno.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include "clientcon.h"
#include <iostream>
using namespace std;

class cServer {
	private:
		int				listenSocket;

		fd_set			master;
		set<int> 		sockets;
	
		FILE*			log;

		bool			checkStdin;
		
		int				addUser();
		void			disconnectUser(vector<int>::iterator iter);
	protected:
		cClientCon*		clients[SERVER_MAX_CLIENTS];
		vector<int>		clientIds;
		set<int>		availableIds;
		char			packet[SERVER_MAX_PACKET_SIZE];
		char			logData[SERVER_MAX_LOG_ENTRY];

		void			addSocket(int s);
		void			sendToLog();
		void			sendToUser(char *data, int len, int user);	
		virtual void	processNewUser(int user);
		virtual void	processNewData(int user);
		virtual void	processDisconnectUser(int user);

		virtual void	allocateClients();
	public:
		cServer();
		~cServer();
		void setLog(FILE *logfile);
		virtual bool init(char *port);

		void update();

		void setStdin(bool b);
};
#endif
