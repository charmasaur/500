#include "client.h"

cClient::cClient() {
	setLog(stdout);
	logData[0]='\0';
	connected=false;
}

void cClient::setLog(FILE *logfile) {
	log=logfile;
}

cClient::~cClient() {
	fclose(log);
	close(theSocket);
}

bool cClient::init(char *ip, char *port) {
	checkStdin=false;
	struct addrinfo hints, *info, *p;
	int status;
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;

	if((status=getaddrinfo(ip,port,&hints,&info))!=0) {
		sprintf(logData, "Getaddrinfo error: %s",gai_strerror(status));
		sendToLog();
		return false;
	}

	theSocket=-1;
	for(p=info;p!=NULL;p=p->ai_next) {
		if((theSocket=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1) continue;
		break;
	}
	if(p==NULL) {
		sprintf(logData, "Couldn't get socket");
		sendToLog();
		freeaddrinfo(info);
		return false;
	}
	sprintf(logData, "Connecting...");
	sendToLog();
	if(connect(theSocket,info->ai_addr,info->ai_addrlen)==-1) {
		sprintf(logData, "Connect error: %s",strerror(errno));
		sendToLog();
		freeaddrinfo(info);
		return false;
	}
	sprintf(logData,"Connected!");
	sendToLog();
	connected=true;

	FD_SET(theSocket,&master);
	
	freeaddrinfo(info);
	return true;
}

void cClient::setStdin(bool b) {
	checkStdin=b;
	if(b) FD_SET(STDIN,&master);
}

void cClient::sendToLog() {
	if(!strlen(logData)) return;
	fprintf(log,"%s\n",logData);
	logData[0]='\0';
}

void cClient::disconnect() {
	close(theSocket);
	connected=false;
}

// TODO: partial send support
void cClient::sendToServer(char *msg, int len) {
	if(send(theSocket,msg,len,0)<0) {
		sprintf(logData,"Send error: %s",strerror(errno));
		sendToLog();
	}
}

void cClient::update() {
	fd_set readfds=master;
	if(select(theSocket+1,&readfds,NULL,NULL,NULL)==-1) {
		sprintf(logData,"Select error");
		sendToLog();
		return;
	}
	if(checkStdin && FD_ISSET(STDIN,&readfds)) {
		//cin.getline(packet,MAX_PACKET_SIZE);
		processNewData(-1);
	}
	if(FD_ISSET(theSocket,&readfds)) {
		if((packetSize=recv(theSocket,packet,MAX_PACKET_SIZE,0))==-1) {
			sprintf(logData,"Recv error: %s",strerror(errno));
			sendToLog();
			return;
		}
		if(packetSize==0) {
			processDisconnect();
			connected=false;
		}
		else {
			processNewData(0);
		}
	}
}

void cClient::processNewData(int user) {
}

void cClient::processDisconnect() {
}
