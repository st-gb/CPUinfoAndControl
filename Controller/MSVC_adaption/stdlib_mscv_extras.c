#include "stdlib_msvc_extras.h"

//from http://www.linuxforums.org/forum/linux-programming-scripting/40146-getch-gcc.html
#include <termios.h>
#include <unistd.h>
int getche(void)
{
  struct termios oldt,
  newt;
  int ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}
