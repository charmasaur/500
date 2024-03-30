#include "input.h"

cInput::cInput() {
	setPos(0,0);
}

cInput::~cInput() {
}

void cInput::insertChar(int ch) { 
	for(int i=nin+1;i>inpstart+curspos;i--) inpline[i]=inpline[i-1];
    inpline[inpstart+curspos]=ch;
    if(curspos==width-1) inpstart++;
    else curspos++;
    nin++;
}

void cInput::removeChar() {
    if(!curspos && !inpstart) return;
    for(int i=inpstart+curspos-1;i<nin;i++) inpline[i]=inpline[i+1];
    nin--;
    if(!curspos) inpstart--;
    else {
        if(inpstart && inpstart+width-1==nin+1) inpstart--;
        else curspos--;
    }
}

void cInput::eraseLine() {
	for(int i=0;i<width;i++) toprint[i]=' ';
	toprint[width]='\0';
	mvprintw(pr,pc,toprint);
	refresh();
}

bool cInput::newChar(int ch) {
	if(ch==8 || ch==127 || ch==KEY_BACKSPACE) {
		removeChar();
	}
	else if(ch==KEY_LEFT) {
		if(curspos==0 && inpstart) inpstart--;
        else if(curspos) curspos--;
    }
    else if(ch==KEY_RIGHT) {
        if(inpstart+curspos<nin) {
            if(curspos==width-1) inpstart++;
            else curspos++;
        }
    }
    else if(ch==KEY_DOWN) {
        reset();
    }
    else if(ch=='\0' || ch=='\n') {
        return true;
    }
    else if(nin<MAX_INPUT-1) {
        insertChar(ch);
    }
    print();
    return false;
}

void cInput::setInput(char *msg) {
	msg[MAX_INPUT-1]='\0';
	strcpy(inpline,msg);
	nin=strlen(inpline);
	curspos=min(strlen(inpline),width-1);
	inpstart=nin-curspos;
	print();
}

void cInput::print() {
	eraseLine();
	int i;
	for(i=0;i<min(width,nin-inpstart);i++) toprint[i]=inpline[inpstart+i];
	toprint[i]='\0';
	mvprintw(pr,pc,toprint);
	move(pr,pc+curspos);
	refresh();
}
