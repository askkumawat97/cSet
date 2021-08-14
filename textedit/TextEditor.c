//
//using NCURSES insertion, replace, deletion
//COMPILE   = gcc texteditor6.c -o texteditor -lncurses
//RUN       = ./texteditor
//


#include<stdio.h>
#include<stdlib.h>
#include<curses.h>

struct linenode { //linklist contains the lines
	struct wordnode *wordlink;
	struct linenode *linelink;
};
struct wordnode { //each line breaked into array of size 10
	char ele[10];
	struct wordnode *link;
};

//below function insert and replace character
void insertchar(struct linenode *linestart, int line, int clmn, char ele, int mode) {
	struct linenode *linenext = NULL;
	struct wordnode *wordnext = NULL;
	linenext = linestart;

	int line1 = 0, clmn1 = 0;
	int i = 0;
	int flag = -1;
	while(line != line1) { //reaches to the given line
		linenext = linenext->linelink;
		line1++;
	}

	if(ele != '\n') { //if not new line add wordnode to linenode
		if(linenext->wordlink == NULL) {
			linenext->wordlink = (struct wordnode*)malloc(sizeof(struct wordnode));
			wordnext = linenext->wordlink;
			wordnext->link = NULL;
			for(int k = 0; k < 10; k++) //initialize to zero
				wordnext->ele[k] = 0;
		}
	}
	wordnext = linenext->wordlink;

	while(1) {
		if(clmn == clmn1) { //a position where character to be insert
			if(ele == '\n') { //if new line
				struct linenode *newline = (struct linenode*)malloc(sizeof(struct linenode));
				newline->linelink = linenext->linelink;
				newline->wordlink = NULL;
				linenext->linelink = newline;
				if(linenext->wordlink == NULL) break;

				struct wordnode *newword = (struct wordnode*)malloc(sizeof(struct wordnode));
				for(int k = 0; k < 10; k++) //initialize to zero
					newword->ele[k] = 0;
				for(int k = i, j = 0; k < 10; k++, j++) {
					newword->ele[j] = wordnext->ele[k];
					wordnext->ele[k] = 0;
				}
				newword->link = wordnext->link;
				wordnext->link = NULL;

				linenext->linelink->wordlink = newword;
				break;
			}
			else if(wordnext->ele[i] == 0 || mode == 0) { //insert or replace character
				wordnext->ele[i] = ele;
				break;
			}
			else { //if inserts into fully filled wordnode then newnode needed
				flag = 0;
				for(int k = 9; k > i; k--)
					if(wordnext->ele[k] == 0) {
						flag =1;
						wordnext->ele[k] = wordnext->ele[k-1];
						wordnext->ele[k-1] = 0;
					}
			}
			if(flag == 1)
				wordnext->ele[i] = ele;
			if(flag == 0) {
				struct wordnode *new = (struct wordnode*)malloc(sizeof(struct wordnode));
				for(int k = 0; k < 10; k++) //initialize to zero
					new->ele[k] = 0;
				for(int k = i, j = 0; k < 10; k++, j++) {
					new->ele[j] = wordnext->ele[k];
					wordnext->ele[k] = 0;
				}
				wordnext->ele[i] = ele;
				new->link = wordnext->link;
				wordnext->link = new;
			}
			break;
		}
		if(wordnext->ele[i] != 0) {
			clmn1++;
		}
		if(i == 9) { //if wordnode filled then newnode needed
			if(wordnext->link == NULL) {
				wordnext->link = (struct wordnode*)malloc(sizeof(struct wordnode));
				wordnext->link->link = NULL;
				wordnext = wordnext->link;
				for(int k = 0; k < 10; k++) //initialize to zero
					wordnext->ele[k] = 0;
			}
			if(wordnext->link != NULL) {
				wordnext = wordnext->link;
			}
			i = -1;
		}
		i++;
	} //end of while(1)
}


void deletechar(struct linenode *linestart, int line, int clmn) {
	struct linenode *linenext = NULL;
	struct wordnode *wordnext = NULL;
	linenext = linestart;

	int line1 = 0, clmn1 = -1;
	int i = -1;

	if(clmn == -1) { //delete at start of line, the line attached to previous line
		if(line != 0) {
			while(line-1 != line1) {
				linenext = linenext->linelink;
				line1++;
			}
			if(linenext->wordlink == NULL)
				linenext->wordlink = linenext->linelink->wordlink;
			else {
				wordnext = linenext->wordlink;
				while(wordnext->link != NULL)
					wordnext = wordnext->link;
				wordnext->link = linenext->linelink->wordlink;
			}
			linenext->linelink = linenext->linelink->linelink;
			//linenext->linelink->wordlink = NULL;
			//linenext->linelink->linelink = NULL;
		}
	}
	else { //delete one character from position
		while(line != line1) {
			linenext = linenext->linelink;
			line1++;
		}
		if(linenext->wordlink != NULL) {
			wordnext = linenext->wordlink;
			while(1) {
				while(++i < 10) {
					if(wordnext->ele[i] != 0) {
						clmn1++;
					}
					if(clmn == clmn1) {
						wordnext->ele[i] = '@';
						wordnext->ele[i] = 0;
						break;
					}
				}
				if(clmn == clmn1) break;
				if(wordnext->link != NULL) {
					wordnext = wordnext->link;
					i = -1;
				}
				else if(wordnext->link == NULL) break;
			}
		}
	}
}



void main() {
	initscr();
	keypad(stdscr, TRUE);
	noecho();
	//cbreak();

	int charcount = 0;
	int wordcount = 0;
	int linecount = 0;

	int ch;
	int mode = 1;//0=replace,1=before,2=after
	int curx = 0;
	int cury = 0;
	int linerange = 0;
	int wordrange = 0;

	struct linenode *linestart = NULL, *linenext = NULL;
	struct wordnode *wordstart = NULL, *wordnext = NULL;
	if(linestart == NULL) {
		linestart = (struct linenode*)malloc(sizeof(struct linenode));
		linestart->linelink = NULL;
	}

	while(1) {
		ch = getch();

		if(ch == 27) { //exit
			break;
		}
		else if(ch == 331) { //replace
			mode = (mode+1)%2;
		}
		else if(ch == KEY_LEFT) {
	                if(curx > 0) curx--;
	        }
	        else if(ch == KEY_RIGHT) {
        	        if(curx < 100) curx++;
        	}
        	else if(ch == KEY_UP) {
        	        if(cury > 0) cury--;
        	}
        	else if(ch == KEY_DOWN) {
        	        if(cury < linerange) cury++;
        	}
		else if(ch == KEY_BACKSPACE) {
			deletechar(linestart, cury, curx-1);
			if(curx != 0)
				charcount--;
			if(curx > 0)
				curx--;
			else if(curx == 0 && cury != 0)
				cury--, linerange--;
		}
		else {
			insertchar(linestart, cury, curx, ch, mode);
			if(ch == '\n') cury++, curx = 0, linerange++;
			else curx += mode, charcount += mode;
		}


		clear();
		move(0, 0);
		//////////////////////////////////////////////////////////////////
		// printing content of data structure.
		linenext = linestart;
		if(linenext != NULL) {
		while(1) {
			if(linenext->wordlink != NULL) {
				wordnext = linenext->wordlink;
				while(1) {
					for(int i = 0; i < 10; i++)
						printw("%c", wordnext->ele[i]);
					if(wordnext->link == NULL) break;
					wordnext = wordnext->link;
				}
			}
			if(linenext->linelink == NULL) break;
			linenext = linenext->linelink;
			printw("\n");
		}
		}
		/////////////////////////////////////////////////////////////////


		move(24, 0);
		printw("BELOW LINES SHOWS HOW THE CHARACTER ARE STORED IN ARRAY OF 10");
		move(25, 0);
		//////////////////////////////////////////////////////////////////
		linenext = linestart;
		if(linenext != NULL) {
		while(1) {
			printw("line:");
			if(linenext->wordlink != NULL) {
				wordnext = linenext->wordlink;
				while(1) {
					printw("_");
					for(int i = 0; i < 10; i++)
						printw("%c", wordnext->ele[i]);
					if(wordnext->link == NULL) break;
					wordnext = wordnext->link;
				}
			}
			if(linenext->linelink == NULL) break;
			linenext = linenext->linelink;
			printw("\n");
		}
		}
		/////////////////////////////////////////////////////////////////

		move(49, 0);
		printw("--------------------------------------");
		move(50, 0);
		printw("%d,%d",cury,curx);
	        printw("    char:%d,line:%d",charcount,linerange+1);
		if(mode == 1) printw("    --insert--");
		else printw("    --replace--");
		move(51, 0);
		printw("--------------------------------------");

		move(cury, curx);
	}
	endwin();

}
