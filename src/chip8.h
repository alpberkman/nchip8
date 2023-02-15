#include <stdint.h>


typedef uint16_t opcode;

typedef struct chip8 {
    uint8_t memory[4096];

    uint16_t PC;	//Program Counter
    uint16_t I;	//Address register

    uint8_t V[16];	//Registers

    uint16_t stack[16];
    uint8_t stack_pointer;


    uint8_t display[64*32];

    char keyboard[16];

    uint8_t delay_timer;
    uint8_t sound_timer;
} chip8;


void init(chip8 *emu);
void load(chip8 *emu, const char* ROM_Address);
opcode fetch(chip8 *emu);
void decodeAndExecute(chip8 *e, opcode instruction);
void cycle(chip8 *emu);




