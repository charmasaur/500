#include "chatserver.h"

void cMyServer::processNewUser(int user) {
	// write send "x joined" to everybody except the guy who joined
	char msg[MAX_NAME_LEN+10];
	sprintf(clientNames[user],"Client%d",user);
	sprintf(msg,"%s joined",clientNames[user]);
	for(int i=0;i<clientIds.size();i++) if(clientIds[i]!=user) sendToUser(msg,strlen(msg)+1,clientIds[i]);
}

void cMyServer::processNewData(int user) {
	char msg[SERVER_MAX_PACKET_SIZE+MAX_NAME_LEN+3];
	if(packet[0]=='\\') {
		sscanf(packet,"\\%s",&msg);
		if(user<0) {
			if(strcmp(msg,"quit")==0) {
				toquit=true;
				return;
			}
		}
		else if(strcmp(msg,"nick")==0) {
			sscanf(packet,"\\nick %s",&msg);
			msg[MAX_NAME_LEN-1]='\0';
			sprintf(packet,"%s changed her name to ",clientNames[user]);
			strcpy(clientNames[user],msg);
			sprintf(msg,"%s%s",packet,clientNames[user]);
		}
	}
	else if(user>=0) sprintf(msg,"<%s> %s",clientNames[user],packet);
	if(user>=0) for(int i=0;i<clientIds.size();i++) sendToUser(msg,strlen(msg)+1,clientIds[i]);
}

void cMyServer::processDisconnectUser(int user) {
	// tell everybody (except user) that user left
	char msg[MAX_NAME_LEN+10];
	sprintf(msg,"%s left",clientNames[user]);
	for(int i=0;i<clientIds.size();i++) if(clientIds[i]!=user) sendToUser(msg,strlen(msg)+1,clientIds[i]);
}
