/* 
	Store a series of lines, and display these to a rectangle on the screen.
*/

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <ncurses.h>
#include <cstring>
#include "../constants.h"
#define MAX_ROWS 512
#define MAX_COLS 512

class cOutput {
	private:
		char lines[MAX_ROWS][MAX_COLS];
		int pr,pc,width,height;
		int numlines;

		void eraseBox();
	public:
		cOutput();
		~cOutput();

		void setPos(int r, int c) {pr=r; pc=c;}
		void setDim(int h, int w) {height=h; width=w; reset();}

		void reset() {numlines=0;}
		
		void addLine(char *line);
		void print();
};

#endif
