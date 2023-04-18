#ifndef GETCH
#define GETCH

//https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux

#include <termios.h>
#include <stdio.h>

static struct termios old, current;

void initTermios(int echo);
void resetTermios(void);
char getch_(int echo);
char getch(void);
char getche(void);

#endif
/// end ///
