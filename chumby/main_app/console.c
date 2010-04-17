#include <stdio.h>
#include <string.h>

void console_clrscr(void) {
      int i;
       
      for (i = 0; i < 25; i++)
      // A bunch of new lines for now. It's blank, hey!
      putchar('\n');
      fflush(stdout);
}

//Y values seem to start at position 1, not 0
int console_gotoxy(int x, int y) {
      char essq[25]; // String variable to hold the escape sequence
      char xstr[25]; // Strings to hold the x and y coordinates
      char ystr[25]; // Escape sequences must be built with characters
       
      /*
      ** Convert the screen coordinates to strings
      */
      sprintf(xstr, "%d", x);
      sprintf(ystr, "%d", y);
      /*
      ** Build the escape sequence (vertical move)
      */
      essq[0] = '\0';
      strcat(essq, "\033[");
      strcat(essq, ystr);
      /*
      ** Described in man terminfo as vpa=\E[%p1%dd
      ** Vertical position absolute
      */
      strcat(essq, "d");
      /*
      ** Horizontal move
      ** Horizontal position absolute
      */
      strcat(essq, "\033[");
      strcat(essq, xstr);
      // Described in man terminfo as hpa=\E[%p1%dG
      strcat(essq, "G");
      /*
      ** Execute the escape sequence
      ** This will move the cursor to x, y
      */
      fprintf(stderr, "%s", essq);
      return 0;
}
