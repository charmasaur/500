#ifndef FCLIENT_H_
#define FCLIENT_H_

#include "../Client/client.h"
#include "500global.h"
#include "../constants.h"
#include "../Curses/input.h"
#include "../Curses/output.h"

class c500Client : public cClient {
	private:
		cInput	input;
		cOutput output;
		char inputSuggestion[MAX_INPUT];

		bool colour;
		bool isSpec;
		int r,c;

		int state;

		int curp;

		int hand[11];
		int scores[2];
		char players[4][MAX_NAME_LEN];
		int trick[5];
		int lasttrick[5];
		int kitty[3];
		bool hasKitty;

		int curbid;
		int trickswon;
		int packetStart;

		char suits[4][16];
		char nums[14][16];

		void setSuggestion();
		
		void getBytes(int num, void *pt);

		void initCards();
		void cardToString(int card, char *str);

		void printScores();
		void printTeams();
		void printTrick();
		void printCards();
		void printBid();
		void printKitty();
		void print();

		void processNewData(int user);
		void processDisconnect();
	public:
		c500Client();
		~c500Client();

		void start();
		void shutdown();

		void setName(char *hi);

		void update();
};

#endif
