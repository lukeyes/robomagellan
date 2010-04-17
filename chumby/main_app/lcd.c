#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h"

// char * rgp format is "rrr,ggg,bbb"were each letter is a base 10 digit
void set_lcd_color(char * rgb) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "imgtool --mode=draw --fill=%s --fb=0 > /dev/null 2>&1", rgb);
    system(cmd);
}

void write_lcd(char * text, int row, int col) {
    char cmd[64];
    //snprintf(cmd, sizeof(cmd), "fbwrite --pos=%d,%d \"%s\n\" 2> /dev/null", col,row,text);
    snprintf(cmd, sizeof(cmd), "fbwriteln %d %s", row,text);
    system(cmd);
}
