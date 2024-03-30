#ifndef FSERVER_H_
#define FSERVER_H_

#include "../Server/server.h"
#include "500global.h"
#include <cmath>

class c500Server : public cServer {
	private:
		char clientNames[SERVER_MAX_CLIENTS+1][MAX_NAME_LEN];
		int clientSpec[SERVER_MAX_CLIENTS+1];
		int hands[4][11];
		int kitty[3];
		int trick[5];
		int players[4];
		bool canbid[4];
		int curbid;
		int trumps;
		int scores[2];
		int trickswon;

		int state;
		int curp;
		int dealer;
		int bidwinner;

		void deal();

		int ctosuit(char c);

		bool checkPlayers();

		void requestBid();
		void processBid(char *msg);
		int nextBidder();
		void finishedBidding();

		bool isValidCard(int card);

		void resetRound();

		void startTrick();
		void requestCard();
		void processCard(char *msg);
		void finishedTrick();

		bool cmpCard(int a, int b, int trump, bool bowers=true);

		bool isPlayer(int user);

		void sendHand(int player);
		void sendKitty(int player);
		void sendTrick();
		void sendScores();
		void sendPlayers();
		void sendBid();
		void sendTricksWon();
		void sendState();
		void sendCurPlayer();

		void sortHand(int wh, int trump);
		void sortHands(int trump);
	
		void startRound();
		void finishedRound();

		void finished(int winner);

		void setPlayer(int user, int wh);

		void sendToAll(char *msg, int len);
		
		void processNewUser(int user);
		void processNewData(int user);
		void processDisconnectUser(int user);
	public:
		c500Server();
		~c500Server();

		int getState() {return state;}
};

#endif
