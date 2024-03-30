#include "500server.h"

c500Server::c500Server() {
	state=WAITING_FOR_PLAYERS;
	for(int i=0;i<4;i++) for(int j=0;j<11;j++) hands[i][j]=0;
	for(int i=0;i<5;i++) trick[i]=0;
	for(int i=0;i<2;i++) scores[i]=0;
	for(int i=0;i<4;i++) players[i]=-1;
	strcpy(clientNames[SERVER_MAX_CLIENTS],"");
	curbid=-1;
	dealer=0;
	curp=0;
}

c500Server::~c500Server() {
}

int c500Server::ctosuit(char c) {
    if(c=='N') return NOTRUMPS;
    if(c=='H') return HEARTS;
    if(c=='D') return DIAMONDS;
    if(c=='C') return CLUBS;
    if(c=='S') return SPADES;
    if(c=='M') return MISERE;
    return NOT;
}

void c500Server::deal() {
	int nc=0;
	int cards[44];
	cards[nc++]=encode(HEARTS,JOKER);
	for(int suit=0;suit<4 && nc<44;suit++) 
		for(int card=3+suit/2;card<=13 && nc<44;card++) cards[nc++]=encode(suit,card);
	if(nc!=43) {
		sprintf(logData,"Wrong number of cards");
		sendToLog();
		state=QUITTING;sendState();
		return;
	}
	int w;
	for(int i=0;i<4;i++) {
		for(int j=0;j<10;j++) {
			w=rand()%nc;
			hands[i][j]=cards[w];
			for(int k=w;k<nc-1;k++) cards[k]=cards[k+1];
			nc--;
		}	
		hands[i][10]=10;
	}
	sortHands(NOTRUMPS);
	if(nc!=3) {
		sprintf(logData,"Wrong number of cards for kitty");
		sendToLog();
		state=QUITTING;sendState();
		return;
	}
	for(int i=0;i<3;i++) kitty[i]=cards[i];
}

void c500Server::sendToAll(char *msg, int len) {
	for(int i=0;i<clientIds.size();i++) sendToUser(msg, len, clientIds[i]);
}

void c500Server::processNewUser(int user) {
	sprintf(clientNames[user],"Player%d",user+1);
	clientSpec[user]=-1;
	char msg[SERVER_MAX_PACKET_SIZE];
	sendPlayers();
	sendScores();
	sendTrick();
	sendBid();
	sendState();
	sendCurPlayer();
	sprintf(msg,"\\info %s joined",clientNames[user]);
	sendToAll(msg,strlen(msg)+1);
//	sprintf(msg,"\\player %p",players);
//	sendToUser(msg,strlen(msg)+1,user);
//	sprintf(msg,"\\scores %p",scores);
//	sendToUser(msg,strlen(msg)+1,user);
//	sprintf(msg,"\\trick %p",trick);
//	sendToUser(msg,strlen(msg)+1,user);
}

bool c500Server::checkPlayers() {
	for(int i=0;i<4;i++) if(players[i]<0) {state=WAITING_FOR_PLAYERS;sendState(); return false;}
//	state=BIDDING;sendState();
	return true;
}

// a>b?
bool c500Server::cmpCard(int a, int b, int trump, bool bowers) {
	int sa=getsuit(a);
	int sb=getsuit(b);
	int na=getnum(a);
	int nb=getnum(b);
	if(sb==NOT) return true;
	if(sa==NOT) return false;
	//if(trump!=NOTRUMP && na==JOKER) return true;
	//if(trump!=NOTRUMP && nb==JOKER) return false;
	
	if(bowers && sa/2==trump/2 && na==JACK){ na=RBOWER-abs(sa-trump); sa=trump;}
	if(bowers && sb/2==trump/2 && nb==JACK){ nb=RBOWER-abs(sb-trump); sb=trump;}

	if(sa==trump) sa=TRUMPS;
	if(sb==trump) sb=TRUMPS;
	
	if(sa<sb) return true;
	if(sb<sa) return false;

	if(na>nb) return true;
	if(nb>na) return false;

	return true;
}

void c500Server::sortHands(int trump) {
	for(int i=0;i<4;i++) sortHand(i,trump);
}

void c500Server::sortHand(int i, int trump) {
	int tmp,cur;
	// INSERTION SORT YEW!
	for(int j=1;j<10;j++) {
		cur=j;
		for(int p=j-1;p>=0;p--) {
			if(cmpCard(hands[i][p],hands[i][cur],trump,trump!=NOTRUMPS)) break;
			tmp=hands[i][p];
			hands[i][p]=hands[i][cur];
			hands[i][cur]=tmp;
			cur=p;
		}
	}
}	

void c500Server::sendHand(int player) {
	char msg[SERVER_MAX_PACKET_SIZE];
	//char tempmsg[16];
	if(players[player]<0) return;
	msg[0]='h';
	memcpy(&(msg[1]),hands[player],11*sizeof(int));
	sendToUser(msg,11*sizeof(int)+1,players[player]);
	msg[0]='x';
	for(int i=0;i<clientIds.size();i++) if(clientSpec[clientIds[i]]>=0 && clientSpec[clientIds[i]]<4) sendToUser(msg,11*sizeof(int)+1,clientIds[i]); 
	//sprintf(msg,"\\hand ");
	//for(int i=0;i<11;i++) {
	//	sprintf(tempmsg," %d",hands[player][i]);
	//	strcat(msg,tempmsg);
	//}
	//sendToUser(msg,strlen(msg)+1,players[player]);
}

void c500Server::sendKitty(int player) {
	char msg[SERVER_MAX_PACKET_SIZE];
	if(players[player]<0) return;
	msg[0]='k';
	memcpy(msg+1,kitty,3*sizeof(int));
	sendToUser(msg,3*sizeof(int)+1,players[player]);
	for(int i=0;i<clientIds.size();i++) if(clientSpec[clientIds[i]]>=0 && clientSpec[clientIds[i]]<4) sendToUser(msg,3*sizeof(int)+1,clientIds[i]); 
}

void c500Server::sendTrick() {
	char msg[SERVER_MAX_PACKET_SIZE];
	msg[0]='t';
	memcpy(&(msg[1]),trick,5*sizeof(int));
	sendToAll(msg,5*sizeof(int)+1);
	//char tempmsg[16];
	//sprintf(msg,"\\trick");
	//for(int i=0;i<5;i++) {
	//	sprintf(tempmsg," %d",trick[i]);
	//	strcat(msg,tempmsg);
	//}
	//sendToAll(msg,strlen(msg)+1);
}

void c500Server::sendScores() {
	char msg[SERVER_MAX_PACKET_SIZE];
	msg[0]='S'; 
	memcpy(msg+1,scores,2*sizeof(int));
	sendToAll(msg,2*sizeof(int)+1);
	//char tempmsg[16];
	//sprintf(msg,"\\scores");
	//for(int i=0;i<2;i++) {
	//	sprintf(tempmsg," %d",scores[i]);
	//	strcat(msg,tempmsg);
	//}
	//sendToAll(msg,strlen(msg)+1);
}

void c500Server::sendPlayers() {
	char msg[SERVER_MAX_PACKET_SIZE];
	msg[0]='p';
	for(int i=0;i<4;i++) 
		memcpy(msg+1+MAX_NAME_LEN*i,(players[i]>=0)?clientNames[players[i]]:clientNames[SERVER_MAX_CLIENTS],MAX_NAME_LEN);
	sendToAll(msg,MAX_NAME_LEN*4+1);
	//memcpy(&(msg[1]),players,4*sizeof(int));
	//sendToAll(msg,4*sizeof(int)+1);
	//char tempmsg[16];
	//sprintf(msg,"\\players");
	//for(int i=0;i<4;i++) {
	//	sprintf(tempmsg," %d",players[i]);
	//	strcat(msg,tempmsg);
	//}
	//sendToAll(msg,strlen(msg)+1);
}

void c500Server::sendBid() {
	char msg[SERVER_MAX_PACKET_SIZE];
	msg[0]='b';
	memcpy(msg+1,&curbid,sizeof(int));
	sendToAll(msg,sizeof(int)+1);
}

void c500Server::sendTricksWon() {
	char msg[SERVER_MAX_PACKET_SIZE];
	msg[0]='w';
	memcpy(msg+1,&trickswon,sizeof(int));
	sendToAll(msg,sizeof(int)+1);
}

void c500Server::sendState() {
	char msg[SERVER_MAX_PACKET_SIZE];
	msg[0]='s';
	memcpy(msg+1,&state,sizeof(int));
	sendToAll(msg,sizeof(int)+1);
}

void c500Server::sendCurPlayer() {
	char msg[SERVER_MAX_PACKET_SIZE];
	msg[0]='c';
	memcpy(msg+1,&curp,sizeof(int));
	sendToAll(msg,sizeof(int)+1);
}

void c500Server::resetRound() {
	curp=dealer;
	sendCurPlayer();
	curbid=-1;
	trickswon=0;
	sendTricksWon();
	sendBid();
	for(int i=0;i<4;i++) {hands[i][10]=0; sendHand(i);}
	trick[4]=0;
	sendTrick();
}

void c500Server::startRound() {
	resetRound();
	char msg[SERVER_MAX_PACKET_SIZE];
	sprintf(msg,"\\info %s's deal",clientNames[players[curp]]);
	sendToAll(msg,strlen(msg)+1);
	deal();
	for(int i=0;i<4;i++) {canbid[i]=true; sendHand(i);}
	state=BIDDING;sendState();
	curp=nextBidder();
	sendCurPlayer();
	requestBid();
}

void c500Server::finishedRound() {
	int bn=getbidnum(curbid);
	int thescore,theextra;
	if(bn==0) { thescore=250; theextra=0; }
	else { thescore=100*(bn-6)+40+20*(3-trumps); theextra=10*(10-trickswon); }
	char msg[SERVER_MAX_PACKET_SIZE];
	char hi[8];
	scores[1-(bidwinner%2)]+=theextra; 
	int winner=-1;
	if((trickswon==0 && bn==0) || (bn>0 && trickswon>=bn)) {
		// won
		scores[bidwinner%2]+=thescore;strcpy(hi,"gains"); if(scores[bidwinner%2]>=500) winner=bidwinner%2;
	}
	else {
		thescore*=-1; scores[bidwinner%2]+=thescore;strcpy(hi,"loses"); if(scores[bidwinner%2]<=-500) winner=1-(bidwinner%2);
	}
	sprintf(msg,"\\info Team %d %s %d points and team %d gains %d points",1+bidwinner%2,hi,abs(thescore),2-(bidwinner)%2,theextra);
	sendToAll(msg,strlen(msg)+1);
	sendScores();
	trick[4]=0;
	sendTrick();
	if(winner>=0) {
		finished(winner);
		return;
	}
	dealer=(dealer+1)%4;
	if(state==PLAYING) startRound();
}

void c500Server::setPlayer(int user, int wh) {
	//char msg[SERVER_MAX_PACKET_SIZE];
	//sprintf(msg,"\\player %d %d",user,wh);
	//sendToAll(msg,strlen(msg)+1);
	players[wh]=user;
	sendPlayers();
}

void c500Server::finished(int winner) {
	char msg[SERVER_MAX_PACKET_SIZE];
	sprintf(msg,"\\info Team %d won %d to %d!",winner+1,scores[winner],scores[1-winner]);
	sendToAll(msg,strlen(msg)+1);
	for(int i=0;i<4;i++) {hands[i][10]=0; sendHand(i);}
	for(int i=0;i<4;i++) players[i]=-1;
	sendPlayers();
	state=WAITING_FOR_PLAYERS;sendState();
	curbid=-1; sendBid();
	for(int i=0;i<2;i++) scores[i]=0;
	sendScores();
	dealer=0;
}	

int c500Server::nextBidder() {
	int nbidders=0;
	for(int i=0;i<4;i++) nbidders+=canbid[i];
	if(nbidders==0) return -1;
	if(nbidders==1 && curbid>=0) return 1337;
	for(int i=1;i<4;i++) if(canbid[(curp+i)%4]) return (curp+i)%4;
	return curp;
}

void c500Server::finishedBidding() {
	char msg[SERVER_MAX_PACKET_SIZE];
	for(int i=0;i<4;i++) if(canbid[i]) curp=bidwinner=i;
	trickswon=0;
	trumps=getbidsuit(curbid);
	// need to set the joker to trump suit
	if(trumps!=NOTRUMPS) {
		for(int i=0;i<4;i++)
			for(int j=0;j<10;j++)
				if(getnum(hands[i][j])==JOKER) hands[i][j]=encode(trumps,JOKER);
		for(int i=0;i<3;i++) if(getnum(kitty[i])==JOKER) kitty[i]=encode(trumps,JOKER);
	}
	for(int i=0;i<4;i++) {
		sortHand(i,trumps);
		sendHand(i);
	}
	sendKitty(curp);
	state=KITTY;sendState();
	sendCurPlayer();
	sprintf(msg,"\\info Waiting for %s...",clientNames[players[curp]]);
	sendToAll(msg,strlen(msg)+1);
	//state=PLAYING;
	//startTrick();
}
 
void c500Server::startTrick() {
	if(hands[0][10]==0 || (getbidnum(curbid)==0 && trickswon)) {
		finishedRound();
		return;
	}
	trick[4]=0;
	sendTrick();
	requestCard();
}

void c500Server::finishedTrick() {
	/*int startsuit=getsuit(trick[0]);
	int bestnum=getnum(trick[0]),winner=0;
	bool TRUMPEd=(startsuit==trumps);
	for(int i=1;i<4;i++) {
		if(getsuit(trick[i])==trumps) {
			if(!TRUMPEd) {
				bestnum=getnum(trick[i]);
				winner=i;
				TRUMPEd=true;
			}
			else if(getnum(trick[i])>bestnum) {
				bestnum=getnum(trick[i]);
				winner=i;
			}
		}
		else if(!TRUMPEd && getsuit(trick[i])==startsuit && getnum(trick[i])>bestnum) {
			bestnum=getnum(trick[i]);
			winner=i;
		}
	}
	// see if the joker got played
	for(int i=0;i<4;i++) if(getsuit(trick[i])==JOKER) winner=i;*/
	bool anytrumps=false;
	if(trumps!=NOTRUMPS) 
		for(int i=0;i<4;i++) 
			if(getsuit(trick[i])==trumps || (getsuit(trick[i])/2==trumps/2 && getnum(trick[i])==JACK)) anytrumps=true; 
	int winner=0;
	int thetrumps=(anytrumps)?trumps:getsuit(trick[0]);
	for(int i=1;i<4;i++) 
		if(cmpCard(trick[i],trick[winner],thetrumps,anytrumps)) winner=i;
	winner=(winner+curp)%4;
	if(winner%2==bidwinner%2) trickswon++;
	sendTricksWon();
	curp=winner;
	sendCurPlayer();
	startTrick();
}

void c500Server::requestCard() {
	if(trick[4]==4) {
		finishedTrick();
		return;
	}
	char msg[SERVER_MAX_PACKET_SIZE];
	sprintf(msg,"\\info %s's turn",clientNames[players[curp]]);
	sendToAll(msg,strlen(msg)+1);
}

// TODO: joker must be first or last of its suit in NT
bool c500Server::isValidCard(int card) {
	int who=curp;
	if(trick[4]==0) return true;
	int cardsuit=getsuit(card);
	/*if(cardsuit==JOKER) {
		if(trumps==NOTRUMPS) return true;
		cardsuit=trumps;
	}*/
	
	int startsuit=getsuit(trick[0]);
	if(trumps!=NOTRUMPS && startsuit/2==trumps/2 && getnum(trick[0])==JACK) startsuit=trumps;
	if(trumps!=NOTRUMPS && cardsuit/2==trumps/2 && getnum(card)==JACK) cardsuit=trumps;
	/*if(startsuit==JOKER) {
		startsuit=trumps;
	}*/
	if(cardsuit==startsuit) return true;
	for(int i=0;i<hands[who][10];i++) {
	//	if(trumps==NOTRUMPS && getnum(hands[who][i])==JOKER) return false;
		cardsuit=getsuit(hands[who][i]);
		if(trumps!=NOTRUMPS && cardsuit/2==trumps/2 && getnum(hands[who][i])==JACK) cardsuit=trumps;
		if(cardsuit==startsuit) return false;
	}
	return true;
}

void c500Server::processCard(char *msg) {
	char nmsg[SERVER_MAX_PACKET_SIZE];
	int wh; int st; char c;
	sscanf(msg,"%d",&wh);
	if(wh<=0 || wh>hands[curp][10]) { // || !isValidCard(hands[curp][wh-1])) {
		sprintf(nmsg,"\\info Invalid card");
		sendToUser(nmsg,strlen(nmsg)+1,players[curp]);
		return;
	}
	if(trumps==NOTRUMPS && getnum(hands[curp][wh-1])==JOKER) {
		if(sscanf(msg,"%d %c",&wh,&c)<2 || (st=ctosuit(c))<HEARTS || st>SPADES) {
			sprintf(nmsg,"\\info Specify a suit");
			sendToUser(nmsg,strlen(nmsg)+1,players[curp]);
			return;
		}
		hands[curp][wh-1]=encode(st,JOKER);
	}
	if(!isValidCard(hands[curp][wh-1])) {
		if(trumps==NOTRUMPS && hands[curp][wh-1]==encode(st,JOKER)) hands[curp][wh-1]=encode(HEARTS,JOKER);
		sprintf(nmsg,"\\info Invalid card: lrn2play");
		sendToUser(nmsg,strlen(nmsg)+1,players[curp]);
		return;
	}
	hands[curp][10]--;
	trick[trick[4]++]=hands[curp][wh-1];
	//sprintf(nmsg,"\\card %d %d",curp,hands[curp][wh-1]);
	//sendToAll(nmsg,strlen(nmsg)+1);
	hands[curp][wh-1]=encode(NOT,0);
	sortHand(curp,trumps);
	sendHand(curp);
	sendTrick();
	curp=(curp+1)%4;
	sendCurPlayer();
	// misere?
	if(getbidnum(curbid)==0 && curp==(bidwinner+2)%4) {
		trick[trick[4]++]=encode(NOT,0);
		sendTrick();
		curp=(curp+1)%4;
	}
	requestCard();
}

void c500Server::requestBid() {
	if(curp==1337) {
		finishedBidding();
		return;
	}
	char msg[SERVER_MAX_PACKET_SIZE];
	if(curp<0) {
		sprintf(msg,"\\info Re-deal!");
		sendToAll(msg,strlen(msg)+1);
		startRound();
		return;
	}
	sprintf(msg,"\\info %s's bid",clientNames[players[curp]]);
	sendToAll(msg,strlen(msg)+1);
}

void c500Server::processBid(char *msg) {
	char nmsg[SERVER_MAX_PACKET_SIZE];
	if(strcmp(msg,"pass")==0) {
		canbid[curp]=false;
		sprintf(nmsg,"\\info %s passed (soft)",clientNames[players[curp]]);
	}
	else {
		char suit;
		int num;
		if(strcmp(msg,"M")==0) {
			if(curbid==-1 || getbidnum(curbid)<7) {
				sprintf(nmsg,"\\info Invalid bid: lrn2play");
				sendToUser(nmsg,strlen(nmsg)+1,players[curp]);
				return;
			}
			suit='M';
			num=7;
		}
		else if((sscanf(msg,"%d %c",&num,&suit)!=2) || ctosuit(suit)==NOT || num<6 || num>10) {
			sprintf(nmsg,"\\info Invalid bid, but now you don't have to play misere");
			sendToUser(nmsg,strlen(nmsg)+1,players[curp]);
			return;
		}
		curbid=encodebid(ctosuit(suit),num);
		sprintf(nmsg,"\\info %s bid %s",clientNames[players[curp]],msg);
	}
	sendToAll(nmsg,strlen(nmsg)+1);	
	sendBid();
	curp=nextBidder();
	sendCurPlayer();
	requestBid();
}	

bool c500Server::isPlayer(int user) {
	for(int i=0;i<4;i++) if(players[i]==user) return true;
	return false;
}

void c500Server::processNewData(int user) {
	char msg[SERVER_MAX_PACKET_SIZE];
	int status;
	if(packet[0]=='\\') {
		sscanf(packet,"\\%s",&msg);
		if(user<0) {
			if(strcmp(msg,"quit")==0) {
				state=QUITTING;sendState();
				return;
			}
		}
		else {
			if(strcmp(msg,"nick")==0) {
				sscanf(packet,"\\nick %s",&msg);
				msg[MAX_NAME_LEN-1]='\0';
				sprintf(packet,"\\info %s changed her name to ",clientNames[user]);
				strcpy(clientNames[user],msg);
				sprintf(msg,"%s%s",packet,clientNames[user]);
				sendToAll(msg,strlen(msg)+1);
				if(isPlayer(user)) sendPlayers();
			}
			else if(state==WAITING_FOR_PLAYERS && strcmp(msg,"spec")==0 && isPlayer(user)) {
				int wh;
				for(wh=0;wh<4;wh++) if(players[wh]==user) break;
				if(wh<4) {
					players[wh]=-1;
					sendPlayers();
				}
			}
			else if(strcmp(msg,"player")==0 && state==WAITING_FOR_PLAYERS) {
				sscanf(packet,"\\player %d",&status);status--;
				if(status>=0 && status<4 && !isPlayer(user) && players[status]==-1) {
					clientSpec[user]=-1;
					setPlayer(user,status);
					if(checkPlayers()) {
						sprintf(msg,"\\info Ready to start");
						sendToAll(msg,strlen(msg)+1);
						startRound();
					}
				}
			}
			else if(strcmp(msg,"bid")==0 && state==BIDDING && user==players[curp]) {
				strcpy(msg,packet+5);
				processBid(msg);
			}
			else if(strcmp(msg,"play")==0 && state==PLAYING && user==players[curp]) {
				processCard(packet+6);
			}
			else if(strcmp(msg,"swap")==0 && state==KITTY && user==players[curp]) {
				int tmp,status1;
				sscanf(packet,"\\swap %d %d",&status,&status1);
				if(status>0 && status<=10 && status1>0 && status1<=3) {
					tmp=kitty[status1-1];
					kitty[status1-1]=hands[curp][status-1];
					hands[curp][status-1]=tmp;
					sortHand(curp,trumps);
					sendHand(curp);
					sendKitty(curp);
				}
			}
			else if(strcmp(msg,"ready")==0 && state==KITTY && user==players[curp]) {
				state=PLAYING;sendState();
				startTrick();
			}
			else if(strcmp(msg,"follow")==0 && !isPlayer(user)) {
				status=-1;
				sscanf(packet,"\\follow %d",&status);status--;
				if(status>=0 && status<4 && players[status]>=0) {clientSpec[user]=status; sendHand(status); }
			}
		}
	}
	else if(packet[0]!='\0'){
		packet[SERVER_MAX_PACKET_SIZE-strlen(clientNames[user])-10]='\0';
		if(packet[0]<'0' || packet[0]>'9') {
			sprintf(msg,"\\info <%s> ",clientNames[user]);
			strcat(msg,packet);
			sendToAll(msg,strlen(msg)+1);
		}
	}
}

void c500Server::processDisconnectUser(int user) {
	char msg[SERVER_MAX_PACKET_SIZE];
	int eek=-1;
	for(int i=0;i<4 && eek<0;i++) 
		if(players[i]==user) eek=i;
	sprintf(msg,"\\info %s left",clientNames[user]);
	if(eek>=0) {
		setPlayer(-1,eek);
		//resetRound();
		if(state==BIDDING) {
			strcat(msg,", so the round will be restarted D:");
			resetRound();
		}
		if(state==PLAYING || state==KITTY) {
			strcat(msg," and forfeited the round like a boss");
			sendToAll(msg,strlen(msg)+1);
			if(getbidnum(curbid)==0) trickswon=10*(bidwinner%2==eek%2);
			else trickswon=10*(bidwinner%2!=eek%2);
			state=WAITING_FOR_PLAYERS; sendState();
			finishedRound();
			return;
		}
		state=WAITING_FOR_PLAYERS; sendState();
	}	
	sendToAll(msg,strlen(msg)+1);
}		
