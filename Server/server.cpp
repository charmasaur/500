#include "server.h"

cServer::cServer() {
	setLog(stdout);
	allocateClients();
	logData[0]='\0';
}

void cServer::setLog(FILE *logfile) {
	log=logfile;
}

cServer::~cServer() {
	while(!clientIds.empty()) disconnectUser(clientIds.begin());
	availableIds.clear();
	sockets.clear();
	close(listenSocket);
	for(int i=0;i<SERVER_MAX_CLIENTS;i++) delete clients[i];
	fclose(log);
}

void cServer::allocateClients() {
	for(int i=0;i<SERVER_MAX_CLIENTS;i++) clients[i]=new cClientCon();
}

bool cServer::init(char *port) {
	checkStdin=false;
	int yes=1;
	addrinfo hints, *info, *p;
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;

	for(int i=0;i<SERVER_MAX_CLIENTS;i++) availableIds.insert(i);

	int status;
	if((status=getaddrinfo(NULL,port,&hints,&info))!=0) {
		sprintf(logData, "Getaddrinfo error: %s", gai_strerror(status));
		sendToLog();
		return false;
	}
	
	listenSocket=-1;
	// Let's bind to a socket
	for(p=info; p!=NULL; p=p->ai_next) {
		if((listenSocket=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1) continue;
		if(setsockopt(listenSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1) {
			sprintf(logData, "Setsockopt error");
			sendToLog();
			freeaddrinfo(info);
			return false;
		}
		if(bind(listenSocket,p->ai_addr,p->ai_addrlen)==-1) continue;
		break;
	}
	if(p==NULL) {
		sprintf(logData, "Binding error");
		sendToLog();
		freeaddrinfo(info);
		return false;
	}
	if(listen(listenSocket,SERVER_BACKLOG)==-1) {
		sprintf(logData,"List error: %s",strerror(errno));
		sendToLog();
		freeaddrinfo(info);
		return false;
	}

	FD_ZERO(&master);
	addSocket(listenSocket);
	
	freeaddrinfo(info);

	return true;
}

void cServer::setStdin(bool b) {
	checkStdin=b;
	if(b && sockets.find(STDIN)==sockets.end()) addSocket(STDIN);
}

void cServer::addSocket(int s) {
	FD_SET(s,&master);
	sockets.insert(-s);
}

void cServer::sendToLog() {
	if(!strlen(logData)) return;
	fprintf(log,"%s\n",logData);
	logData[0]='\0';
}

// TODO: partial send support
void cServer::sendToUser(char *data, int len, int user) {
	if(user<0 || user>=SERVER_MAX_CLIENTS || availableIds.find(user)!=availableIds.end()) {
		sprintf(logData,"Tried to send to non-existent user %d",user);
		sendToLog();
		return;
	}
	int status;
	if((status=send(clients[user]->getSocket(),data,len,0))<0) {
		sprintf(logData,"Send error: %s",strerror(errno));
		sendToLog();
	}
	if(status!=len) {
		sprintf(logData,"Incomplete send. %d/%d",status,len);
		sendToLog();
	}
}

void cServer::update() {
	fd_set readfds=master;
	int ind, status;
	vector<int>::iterator iter;
	if(select(1-(*(sockets.begin())),&readfds,NULL,NULL,NULL)==-1) {
		sprintf(logData,"Select error");
		sendToLog();
		return;
	}
	if(!availableIds.empty() && FD_ISSET(listenSocket,&readfds))
		if((status=addUser())>=0){ processNewUser(status);}
	if(checkStdin && FD_ISSET(STDIN,&readfds)) {
		cin.getline(packet,SERVER_MAX_PACKET_SIZE);
		processNewData(-1);
	}
	for(iter=clientIds.begin(); iter!=clientIds.end(); iter++) {
		ind=*iter;
		if(FD_ISSET(clients[ind]->getSocket(),&readfds)) {
			if((status=recv(clients[ind]->getSocket(),packet,SERVER_MAX_PACKET_SIZE,0))==-1) {
				sprintf(logData,"Recv error: %s",strerror(errno));
				sendToLog();
				continue;
			}
			if(status==0) {
				processDisconnectUser(ind);
				disconnectUser(iter);
				iter--;
			}
			else {
				processNewData(ind);
			}
		}
	}
}

int cServer::addUser() {
	int nu=*(availableIds.begin());
	int status;
	if((status=accept(listenSocket,(sockaddr*)(clients[nu]->getAddr()),clients[nu]->getLen()))==-1) {
		sprintf(logData,"Accept error: %s",strerror(errno));
		sendToLog();
		return -1;
	}
	clients[nu]->setSocket(status);
	addSocket(status);
	clientIds.push_back(nu);
	availableIds.erase(availableIds.begin());
	return nu;
}

void cServer::disconnectUser(vector<int>::iterator iter) {
	close(clients[*iter]->getSocket());
	FD_CLR(clients[*iter]->getSocket(),&master);
	availableIds.insert(*iter);
	sockets.erase(clients[*iter]->getSocket());
	clientIds.erase(iter);
}

void cServer::processNewUser(int user) {
}

void cServer::processNewData(int user) {
}

void cServer::processDisconnectUser(int user) {
}
