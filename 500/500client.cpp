#include "500client.h"

c500Client::c500Client() {
	strcpy(inputSuggestion,"");
	state=WAITING_FOR_PLAYERS;
	curp=0;
	hasKitty=false;
	packetStart=0;
	lasttrick[4]=trick[4]=0;
	for(int i=0;i<4;i++) strcpy(players[i],"");
	curbid=-1;
	initCards();
	trickswon=0;
	hand[10]=0;
	isSpec=true;
}

void c500Client::start() {
	initscr();
	noecho();
	cbreak();
	keypad(stdscr,TRUE);
	colour=(has_colors()==TRUE);
	if(colour) {
		start_color();
		init_pair(1,COLOR_MAGENTA,COLOR_BLACK);
		init_pair(2,COLOR_WHITE,COLOR_BLACK);
		attron(COLOR_PAIR(2));
	}
	getmaxyx(stdscr,r,c);
	
	output.setPos(13,0);
	output.setDim(r-15,c);

	input.setPos(r-2,0);
	input.setWidth(c);
	print();
}

void c500Client::shutdown() {
	attroff(COLOR_PAIR(2));
	endwin();
}

c500Client::~c500Client() {
}

void c500Client::initCards() {
	strcpy(suits[HEARTS],"Hearts");
	strcpy(suits[DIAMONDS],"Diamonds");
	strcpy(suits[CLUBS],"Clubs");
	strcpy(suits[SPADES],"Spades");
	for(int i=0;i<10;i++) sprintf(nums[i],"%d",i+1);
	strcpy(nums[JACK],"Jack");
	strcpy(nums[QUEEN],"Queen");
	strcpy(nums[KING],"King");
	strcpy(nums[ACE],"Ace");
}

void c500Client::setSuggestion() {
	switch(state) {
		case WAITING_FOR_PLAYERS:
			strcpy(inputSuggestion,"\\player ");
			break;
		case BIDDING:
			if(hand[10] && !isSpec) strcpy(inputSuggestion,"\\bid ");
			else if(isSpec) strcpy(inputSuggestion,"\\follow ");
			else inputSuggestion[0]='\0';
			break;
		case PLAYING:
			if(hand[10] && !isSpec) strcpy(inputSuggestion,"\\play ");
			else if(isSpec) strcpy(inputSuggestion,"\\follow ");
			else inputSuggestion[0]='\0';
			break;
		case KITTY:
			if(hasKitty && !isSpec) strcpy(inputSuggestion,"\\swap ");
			else if(isSpec) strcpy(inputSuggestion,"\\follow ");
			else inputSuggestion[0]='\0';
			break;
		default:
			inputSuggestion[0]='\0';
	}
	if(*(input.getInput())!='\\' && *(input.getInput())!='\0') return;
	input.setInput(inputSuggestion);
}

void c500Client::cardToString(int card, char *str) {
	int suit=getsuit(card);
	int num=getnum(card);
	if(suit<HEARTS || suit>SPADES || num<0 || num>JOKER) {
		sprintf(str," - ");//%d of %d",num,suit);
		return;
	}
	if(num==JOKER) strcpy(str,"Joker");
	else if(num<=ACE) sprintf(str,"%s of %s",nums[num],suits[suit]);
}

void c500Client::printScores() {
	char text[32];
	for(int i=0;i<2;i++) {
		sprintf(text,"Team %d: %d",i+1,scores[i]);
		mvprintw(i,50,text);
	}
}

void c500Client::printTeams() {
	char text[32];
	int hi;
	for(int i=0;i<4;i++) {
		hi=(i==curp)*(1-colour);
		if(i==curp) {
			if(!colour) strcpy(text,"*");
			else attron(COLOR_PAIR(1));
		}
		sprintf(text+hi,"Player %d: %s",i+1,players[i]);
		mvprintw(3+i,50,text);
		if(i==curp && colour) attroff(COLOR_PAIR(1));
	}
}

void c500Client::printTrick() {
	char text[64];
	if(state==KITTY) return;
	if(lasttrick[4]==4) {
		mvprintw(6,25,"Last trick:");
		for(int i=0;i<4;i++) {
			cardToString(lasttrick[i],text);
			if(getnum(lasttrick[i])==JOKER) sprintf(text+strlen(text)," (%s)",suits[getsuit(lasttrick[i])]);
			mvprintw(7+i,25,text);
		}
	}
	if(trick[4]) {
		mvprintw(0,25,"This trick:");
		for(int i=0;i<trick[4];i++) {
			cardToString(trick[i],text);
			if(getnum(trick[i])==JOKER) sprintf(text+strlen(text)," (%s)",suits[getsuit(trick[i])]);
			mvprintw(1+i,25,text);
		}
	}
}

void c500Client::printCards() {
	char text[32];
	if(!hand[10]) return;
	sprintf(text,"Your hand:");
	mvprintw(0,0,text);
	for(int i=0;i<hand[10];i++) {
		sprintf(text,"%d: ",i+1);
		cardToString(hand[i],text+strlen(text));
		mvprintw(1+i,0,text);
	}
}

void c500Client::printBid() {
	char text[32];
	if(curbid<0) strcpy(text,"Bid: -");
	else {
		 if(getbidnum(curbid)==0) sprintf(text,"Bid: Misere");
		else sprintf(text,"Bid: %d %s",getbidnum(curbid),(getbidsuit(curbid)==NOTRUMPS)?("No Trumps"):suits[getbidsuit(curbid)]);
		if(trickswon) sprintf(text+strlen(text)," (%d)",trickswon);
	}
	mvprintw(8,50,text);
}

void c500Client::printKitty() {
	if(!hasKitty || state!=KITTY) return;
	char text[32];
	mvprintw(0,25,"Kitty:");
	for(int i=0;i<3;i++) {
		sprintf(text,"%d: ",i+1);
		cardToString(kitty[i],text+strlen(text));
		mvprintw(1+i,25,text);
	}
}

void c500Client::print() {
	erase();
	for(int i=0;i<c;i++) mvprintw(12,i,"-");
	output.print();
	// need to print scores, team, trick, bids and cards
	printScores();
	printTeams();
	refresh();
	printTrick();
	printCards();
	printBid();
	printKitty();
	input.print();
	refresh();
}

void c500Client::getBytes(int num, void *pt) {
	if(packetSize-packetStart<num) return;
	memcpy(pt,packet+packetStart,num);
}

void c500Client::setName(char *hi) {
	char msg[MAX_PACKET_SIZE];
	if(strlen(hi)>=MAX_PACKET_SIZE-9) hi[MAX_PACKET_SIZE-9]='\0';
	sprintf(msg,"\\nick %s\n",hi);
	sendToServer(msg,strlen(msg)+1);
}

void c500Client::processNewData(int user) {
	if(packetStart>=packetSize) {
		packetStart=0;
		print();
		return;
	}
	// -1 = stdin
	// 0 = server
	if(user==-1) {
		if(input.newChar(getch())) {
			if(strcmp(input.getInput(),"\\quit")==0) disconnect();
			else if(*(input.getInput())!='\0') 
				sendToServer(input.getInput(),strlen(input.getInput())+1);
			input.reset();
			input.setInput(inputSuggestion);
		}
	}
	else if(user==0) {
		char msg[MAX_PACKET_SIZE];
		int status;
		if(packet[packetStart]=='\\') {
			sscanf(packet+packetStart,"\\%s",&msg);
			if(strcmp(msg,"info")==0) {
				output.addLine(packet+packetStart+6);
				packetStart+=strlen(packet+packetStart+6)+6;
			}
			else packetStart++;
			processNewData(user);
			return;
		}
		packetStart++;
		if(packet[packetStart-1]=='h' || packet[packetStart-1]=='x') { 
			isSpec=packet[packetStart-1]=='x';
			getBytes(11*sizeof(int),hand);
			packetStart+=11*sizeof(int);
		}
		else if(packet[packetStart-1]=='t') {
			getBytes(5*sizeof(int),trick);
			packetStart+=5*sizeof(int);
			if(trick[4]==4) {
				memcpy(lasttrick,trick,5*sizeof(int));
			}
		}
		else if(packet[packetStart-1]=='S') { 
			getBytes(2*sizeof(int),scores);
			packetStart+=2*sizeof(int);
		}
		else if(packet[packetStart-1]=='p') {
			for(int i=0;i<4;i++) {
				getBytes(MAX_NAME_LEN,players[i]);
				packetStart+=MAX_NAME_LEN;
			}
		}
		else if(packet[packetStart-1]=='b') {
			getBytes(sizeof(int),&curbid);
			packetStart+=sizeof(int);
		}
		else if(packet[packetStart-1]=='w') {
			getBytes(sizeof(int),&trickswon);
			packetStart+=sizeof(int);
		}
		else if(packet[packetStart-1]=='k') {
			hasKitty=true;
			getBytes(3*sizeof(int),kitty);
			packetStart+=3*sizeof(int);
		}
		else if(packet[packetStart-1]=='s') {
			getBytes(sizeof(int),&state);
			if(state!=KITTY) hasKitty=false;
			// if(state!=PLAYING) lasttrick[4]=0;
			packetStart+=sizeof(int);
			setSuggestion();
		}
		else if(packet[packetStart-1]=='c') {
			getBytes(sizeof(int),&curp);
			packetStart+=sizeof(int);
		}
		processNewData(user);
	}
}

void c500Client::processDisconnect() {
}

void c500Client::update() {
	if(is_term_resized(r,c)) {
		getmaxyx(stdscr,r,c);
		input.setPos(r-2,0);
		input.setWidth(c);
		input.setInput(inputSuggestion);
		output.setDim(r-15,c);
		print();
	}
	cClient::update();
}
