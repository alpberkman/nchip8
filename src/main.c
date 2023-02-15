#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"
#include "term_fb.h"

//Components
chip8 emu;


//Key translations
char original[16] = {
    0x01, 0x02, 0x03, 0x0C,
    0x04, 0x05, 0x06, 0x0D,
    0x07, 0x08, 0x09, 0x0E,
    0x0A, 0x00, 0x0B, 0x0F
};

char translation[16] = {
    '1', '2', '3', '4',
    'q', 'w', 'e', 'r',
    'a', 's', 'd', 'f',
    'z', 'x', 'c', 'v'
};

int handleArguments(int argc, char **argv);
void matchKeys(int c);
void debug(uint16_t x_offset, uint16_t y_offset);


int main(int argc, char **argv)
{
    if(handleArguments(argc, argv))
        return -1;

    init(& emu);
    load(&emu, argv[1]);
    setupDisplay(64, 32, emu.display);

    int i;
    while((i = getch()) != '.') {

        matchKeys(i);
        cycle(&emu);

        updateDisplay();

        debug(0, 32);
        refresh();

        usleep(1600);
    }
    closeDisplay();

    return 0;
}


int handleArguments(int argc, char **argv) {
    if(argc != 2 && argc != 3) {
        printf("Please give a ROM\n");
        return 1;
    }

    if(argc == 3) {
        for(int i = 0; i < strlen(argv[2]); ++i)
            translation[i] = argv[2][i];
    }

    return 0;
}

void matchKeys(int c) {

    if(c == ERR)
        return;

    for(uint8_t i = 0; i < 16; ++i) {
        if(((char) c) == translation[i]) {
            emu.keyboard[original[i]] = 1;
        } else {
            emu.keyboard[original[i]] = 0;
        }
    }
}

void debug(uint16_t x_offset, uint16_t y_offset) {

    for(uint8_t i = 0; i < 8; ++i) {
        mvprintw(y_offset + i, x_offset, "V%x:%#04x", i, emu.V[i]);
        mvprintw(y_offset + i, x_offset+9, "V%x:%#04x", i+8, emu.V[i+8]);
    }

    //mvprintw(y_offset, x_offset+18, "opcode:%#06x", inst);
    mvprintw(y_offset+1, x_offset+18, "PC:%#06x", emu.PC);
    mvprintw(y_offset+2, x_offset+18, "I:%#06x", emu.I);

    mvprintw(y_offset+4, x_offset+18, "sound:%#04x", emu.sound_timer);
    mvprintw(y_offset+5, x_offset+18, "delay:%#04x", emu.delay_timer);


    mvprintw(y_offset, 33, "Stack Pointer:%#04x", emu.stack_pointer);
    for(uint8_t i = 0; i < 8; ++i) {
        mvprintw(y_offset+1 + i, x_offset+33, "S%x:%#06x", i, emu.stack[i]);
        mvprintw(y_offset+1 + i, x_offset+44, "S%x:%#06x", i+8, emu.stack[i+8]);
    }

    mvprintw(y_offset, 55, "Keyboard");
    mvprintw(y_offset+1, x_offset+55, "%x", emu.keyboard[0x01]);
    mvprintw(y_offset+1, x_offset+55 + 2, "%x", emu.keyboard[0x02]);
    mvprintw(y_offset+1, x_offset+55 + 4, "%x", emu.keyboard[0x03]);
    mvprintw(y_offset+1, x_offset+55 + 6, "%x", emu.keyboard[0x0C]);
    mvprintw(y_offset+2, x_offset+55, "%x", emu.keyboard[0x04]);
    mvprintw(y_offset+2, x_offset+55 + 2, "%x", emu.keyboard[0x05]);
    mvprintw(y_offset+2, x_offset+55 + 4, "%x", emu.keyboard[0x06]);
    mvprintw(y_offset+2, x_offset+55 + 6, "%x", emu.keyboard[0x0D]);
    mvprintw(y_offset+3, x_offset+55, "%x", emu.keyboard[0x07]);
    mvprintw(y_offset+3, x_offset+55 + 2, "%x", emu.keyboard[0x08]);
    mvprintw(y_offset+3, x_offset+55 + 4, "%x", emu.keyboard[0x09]);
    mvprintw(y_offset+3, x_offset+55 + 6, "%x", emu.keyboard[0x0E]);
    mvprintw(y_offset+4, x_offset+55, "%x", emu.keyboard[0x0A]);
    mvprintw(y_offset+4, x_offset+55 + 2, "%x", emu.keyboard[0x00]);
    mvprintw(y_offset+4, x_offset+55 + 4, "%x", emu.keyboard[0x0B]);
    mvprintw(y_offset+4, x_offset+55 + 6, "%x", emu.keyboard[0x0F]);
}




