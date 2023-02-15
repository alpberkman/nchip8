#include "term_fb.h"


static int x;
static int y;
static char *fb;


void setupNC() {
    initscr();
    cbreak();
    noecho();

    nodelay(stdscr, TRUE);

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
}

void closeNC() {
    endwin();
}


void setupDisplay(int x_axis, int y_axis, char *arr) {
    setupNC();
    x = x_axis;
    y = y_axis;
    fb = arr;
}

void updateDisplay() {
    for(int i = 0; i < y; ++i)
        for(int j = 0; j < x; ++j) {
            if(fb[j + x*i]) {
                attron(COLOR_PAIR(1));
                mvprintw(i, j*2, "  ");
                attroff(COLOR_PAIR(1));
            } else {
                attron(COLOR_PAIR(2));
                mvprintw(i, j*2, "  ");
                attroff(COLOR_PAIR(2));
            }
        }
    refresh();
}

void closeDisplay() {
    closeNC();
}



















