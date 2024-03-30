/*
	Record curses text input and echo it to a specified spot on the screen
*/
#ifndef INPUT_H_
#define INPUT_H_

#define MAX_INPUT 512
#include <ncurses.h>
#include <cstring>
#include "../constants.h"

class cInput {
	private:
		int nin;
		int curspos;
		int inpstart;
		char inpline[MAX_INPUT];
		char toprint[MAX_INPUT];
		int pr,pc,width;

		void insertChar(int ch);
		void removeChar();
		void eraseLine();
	public:
		cInput();
		~cInput();

		void setPos(int r, int c) {reset(); pr=r;pc=c;}
		void setWidth(int w) {reset(); width=w;}
		bool newChar(int ch);

		void reset() {nin=curspos=inpstart=0; inpline[0]='\0';print();}
	
		char *getInput() {return inpline;}

		void setInput(char *str);

		void print();
};

#endif
