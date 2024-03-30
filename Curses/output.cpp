#include "output.h"

cOutput::cOutput() {
	setPos(0,0);
	setDim(10,10);
}

cOutput::~cOutput() {
}

void cOutput::eraseBox() {
	for(int i=0;i<height;i++) for(int j=0;j<width;j++) mvprintw(pr+i,pc+j," ");
}

void cOutput::addLine(char *line) {
	int nlines=ceildiv(strlen(line),width);
    int moveback=max(0,numlines+nlines-(height-1));
    int pos=0;
    int j;
	// TODO: make it not split words across lines
    for(int i=0;i<nlines;i++) {
        for(j=0;j<width && pos<strlen(line);j++)
            lines[numlines+i][j]=line[pos++];
        lines[numlines+i][j]='\0';
    }
    numlines=min(numlines+nlines,height-1);
    if(moveback) for(int i=0;i<numlines;i++) strcpy(lines[i],lines[i+moveback]);
    print();
}

void cOutput::print() {
	eraseBox();
	for(int i=0;i<numlines;i++) mvprintw(pr+i,pc,lines[i]);
	refresh();
}
