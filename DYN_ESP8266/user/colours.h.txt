#ifndef __colours
#define __colours

 #define CC_RESET	0
 #define CC_BRIGHT 	1
 #define CC_DIM		2
 #define CC_UNDERLINE 	3
 #define CC_BLINK	4
 #define CC_REVERSE	7
 #define CC_HIDDEN	8

 #define CC_BLACK 	0
 #define CC_RED		1
 #define CC_GREEN	2
 #define CC_YELLOW	3
 #define CC_BLUE	4
 #define CC_MAGENTA	5
 #define CC_CYAN	6
 #define CC_WHITE  	7

 void gotoxy(int x, int y);
 void clrscr(void);
 void textcolor(int i);
 void textbackground(int i);
 void consoleSize(int * x, int * y) ;
 void textColouring(int attr, int fg, int bg);
 void textattr(int attr) ;
 void showCursor(int yesno);
#endif
