#include <stdlib.h>
#include <stdio.h>

#include "chip8.h"

//Fonts
uint8_t font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void init(chip8 *emu) {

    //Clean memory
    for(int i = 0; i < 4096; ++i)
        emu->memory[i] = 0;

    emu->PC = 0x0200;
    emu->I = 0;

    //Clean registers
    for(int i = 0; i < 16; ++i)
        emu->V[i] = 0;

    //Clean stack
    emu->stack_pointer = 0;
    for(int i = 0; i < 16; ++i)
        emu->stack[i] = 0;

    //Clean display
    for(int i = 0; i < 64*32; ++i)
        emu->display[i] = 0;

    //Reset keyboard
    for(int i = 0; i < 16; ++i)
        emu->keyboard[i] = 0;

    //Reset sound
    emu->delay_timer = 0;
    emu->sound_timer = 0;

    //Load font
    for(int i = 0; i < 80; ++i)
        emu->memory[i] = font[i];

    srand(0);
}

void load(chip8 *emu, const char* ROM_Address) {
    FILE *f_ptr = fopen(ROM_Address, "rb");

    if(f_ptr == NULL) {
        printf("Couldnt read file.");
        return;
    }

    int i;
    while((i = getc(f_ptr)) != EOF)
        emu->memory[emu->PC++] = (uint8_t) i;

    fclose(f_ptr);
    emu->PC = 0x0200;
}

opcode fetch(chip8 *emu) {
    return (((opcode) emu->memory[emu->PC++]) << 8) | emu->memory[emu->PC++];
}

void decodeAndExecute(chip8 *emu, opcode instruction) {

    uint8_t X = (instruction & 0x0F00) >> 8;
    uint8_t Y = (instruction & 0x00F0) >> 4;
    uint8_t N = (instruction & 0x000F);
    uint8_t NN = (instruction & 0x00FF);
    uint16_t NNN = (instruction & 0x0FFF);

    switch((instruction & 0xF000) >> 12) {
    case 0x00:
        switch(NNN) {
        case 0x00E0:
            for(uint16_t i = 0; i < (64*32); ++i)
                emu->display[i] = 0;
            break;
        case 0x00EE:
            emu->stack_pointer -= 1;
            emu->PC = emu->stack[emu->stack_pointer];
            break;
        default:
            break;
        }
        break;
    case 0x01:
        emu->PC = NNN;
        break;
    case 0x02:
        emu->stack[emu->stack_pointer] = emu->PC;
        emu->stack_pointer += 1;
        emu->PC = NNN;
        break;
    case 0x03:
        if(emu->V[X] == NN)
            emu->PC += 2;
        break;
    case 0x04:
        if(emu->V[X] != NN)
            emu->PC += 2;
        break;
    case 0x05:
        if(emu->V[X] == emu->V[Y])
            emu->PC += 2;
        break;
    case 0x06:
        emu->V[X] = NN;
        break;
    case 0x07:
        emu->V[X] += NN;
        break;
    case 0x08:
        switch(N) {
        case 0x00:
            emu->V[X] = emu->V[Y];
            break;
        case 0x01:
            emu->V[X] |= emu->V[Y];
            break;
        case 0x02:
            emu->V[X] &= emu->V[Y];
            break;
        case 0x03:
            emu->V[X] ^= emu->V[Y];
            break;
        case 0x04:
            emu->V[X] += emu->V[Y];
            if(emu->V[X] < emu->V[Y])
                emu->V[0xF] = 1;
            else
                emu->V[0xF] = 0;
            break;
        case 0x05:
            if(emu->V[X] >= emu->V[Y])
                emu->V[0xF] = 1;
            else
                emu->V[0xF] = 0;
            emu->V[X] -= emu->V[Y];
            break;
        case 0x06:
            emu->V[0xF] = 0x1 & emu->V[X];
            emu->V[X] >>= 1;
            break;
        case 0x07:
            if(emu->V[X] <= emu->V[Y])
                emu->V[0xF] = 1;
            else
                emu->V[0xF] = 0;
            emu->V[X] = emu->V[Y] - emu->V[X];
            break;
        case 0x0E:
            emu->V[0xF] = emu->V[X] >> 7;
            emu->V[X] <<= 1;
            break;
        default:
            break;
        }
        break;
    case 0x09:
        if(emu->V[X] != emu->V[Y])
            emu->PC += 2;
        break;
    case 0x0A:
        emu->I = NNN;
        break;
    case 0x0B:
        emu->PC = NNN + emu->V[0x0];
        break;
    case 0x0C:
        emu->V[X] = NN & (rand() % 0xFF);
        break;
    case 0x0D:
    {
        uint8_t x = emu->V[X] & 0x3F;
        uint8_t y = emu->V[Y] & 0x1F;
        emu->V[0xF] = 0;

        for(uint8_t i = 0; i < N && (y+i) < 32 ; ++i) {

            uint8_t row = emu->memory[emu->I + i];

            for(uint8_t j = 0; j < 8 && (x+j) < 64; ++j) {

                uint8_t pixel = (0x80 & row) >> 7;
                row <<= 1;

                if(pixel == 1) {
                    uint16_t pixel_pointer = x + j + 64*(y + i);
                    if(emu->display[pixel_pointer] == 1)
                        emu->V[0xF] = 1;
                    emu->display[pixel_pointer] ^= pixel;
                }
            }
        }
    }
    break;
    case 0x0E:
        switch(NN) {
        case 0x9E:
            if(emu->keyboard[X])
                emu->PC += 2;
            break;
        case 0xA1:
            if(!emu->keyboard[X])
                emu->PC += 2;
            break;
        default:
            break;
        }
        break;
    case 0x0F:
        switch(NN) {
        case 0x07:
            emu->V[X] = emu->delay_timer;
            break;
        case 0x0A:
        {
            int decrease = 1;
            for(uint8_t i = 0; i < 16; ++i)
                if(emu->keyboard[i]) {
                    emu->V[X] = i;
                    decrease = 0;
                    break;
                }
            if(decrease)
                emu->PC -= 2;
        }
        break;
        case 0x15:
            emu->delay_timer = emu->V[X];
            break;
        case 0x18:
            emu->sound_timer = emu->V[X];
            break;
        case 0x1E:
            emu->I += emu->V[X];
            emu->V[0xF] = (emu->I & 0xF000) != 0;
            break;
        case 0x29:
            emu->I = emu->V[X] * 5;
            break;
        case 0x33:
            emu->memory[emu->I] = (emu->V[X] / 100) % 10;
            emu->memory[emu->I+1] = (emu->V[X] / 10) % 10;
            emu->memory[emu->I+2] = (emu->V[X] / 1) % 10;
            break;
        case 0x55:
            for(uint8_t i = 0; i <= X; ++i)
                emu->memory[emu->I+i] = emu->V[i];
            break;
        case 0x65:
            for(uint8_t i = 0; i <= X; ++i)
                emu->V[i] = emu->memory[emu->I+i];
            break;
        default:
            break;
        }
        break;
    default:
        printf("Unknown opcode");
    }
}

void cycle(chip8 *emu) {

    decodeAndExecute(emu, fetch(emu));

    if(emu->delay_timer > 0)
        emu->delay_timer -= 1;

    if(emu->delay_timer > 0)
        emu->sound_timer -= 1;
}






