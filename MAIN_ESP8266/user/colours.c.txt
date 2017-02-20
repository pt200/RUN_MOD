#include <stdio.h>
#include <sys/ioctl.h> // for terminal info
/*

<ESC>[{attr};{fg};{bg}m

{attr} is one of
0 Reset All Attributes (Comes to Nomal mode)
1 Bright (Usually turns on the BOLD)
2 Dim
3 Underline
5 Blink
7 Reverse
8 hidden


blackfore   \e[30m
blackback   \e[40m

redfore     \e[31m
redback     \e[41m

greenfore   \e[32m
greenback   \e[42m

yellowfore  \e[33m
yellowback  \e[43m

bluefore    \e[34m
blueback    \e[44m

magentafore \e[35m
magentaback \e[45m

cyanfore    \e[36m
cyanback    \e[46m

whitefore   \e[37m
whiteback   \e[47m



*/

#include "colours.h"

void gotoxy(int x, int y){ 
  printf("\e[%d;%df", x, y);
 // fflush(stdout);
  return;
}
 
void clrscr(void){ 
  printf("\e[2J");
  return; 
}

void showCursor(int yesno) {
  if (yesno) {
    printf("\e[?25h");
  } else {
    printf("\e[?25l");
  }
}

void textattr(int attr) {
  textColouring(CC_BRIGHT, attr&7, (attr>>4)&7 );
}

void textColouring(int attr, int fg, int bg) {
  char command[16];

  printf("\e[%d;%d;%dm", attr, fg + 30, bg + 40);
}

void textcolor(int i){ 
  int colours[]={30,31,32,33,34,35,36,37};
  if (i>7 )
    printf("\e[1;%dm", colours[i-7]); 
  else 
    printf("\e[22;%dm", colours[i]);
  return; 
}

void textbackground(int i){ 
  int colours[]={40,41,42,43,44,45,46,47};
  if (i>7 )
    printf("\e[1;%dm", colours[i-7]); 
  else 
    printf("\e[22;%dm", colours[i]);
  return; 
}

void consoleSize(int * x, int * y) {

  struct winsize w;
  
  *x = -1;
  *y = -1;
  
  if (ioctl (0, TIOCGWINSZ, &w) >= 0) {
   *x = w.ws_col;
   *y = w.ws_row;
  }   

  return ;
}

/*

getch(){

   select(stdin)
}

*/

/*
int main (void) {
  // printf("\e[2J");
  // printf("\e[5;6f");
  // printf("\e[51mHello  \e[32m,   \e[34mBlue \e[32;6mWorld    \e[0m.");
  // printf("\e[7;2f");
  // printf("\e[5;32m");
  // printf("_");
  
  clrscr();
  gotoxy(4, 10);
  textcolor(7);
  textbackground(4);
  printf("Hello\n");
  
  return 0;
}

*/
