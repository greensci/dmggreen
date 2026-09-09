#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>
#include "bus.h"
#include "cpu.h"
#include "instructions.h"

uint8_t memory[0x10000];

uint8_t ReadBus(uint16_t addr) {
	if (addr == 0xFF44) {
		return 0x90;
	}else
	return memory[addr];
}
void WriteBus(uint16_t addr, uint8_t val) {
	if (addr == 0xDFF1) {
		uint8_t ggaga = val;
	}
	memory[addr] = val;
}
static char dbg_msg[1024] = { 0 };
static int msg_size = 0;

void dbg_update() {
    //printf("dbg %04X\n", bus.bus_read(0xFF02));
   // printf("DEBUG: %02x", bus.bus_read(0xFF02));

    if (ReadBus(0xFF02) == 0x81) {
        char c = ReadBus(0xFF01);
        //printf("DEBUG: %s", c);
        dbg_msg[msg_size++] = c;


        WriteBus(0xFF02, 0x00);
    }

}

void dbg_print() {
    if (dbg_msg[0]) {
        printf("Serial Bus: %s\n", dbg_msg);
    }
}

void CheckInterrupts() {
	uint8_t IE = ReadBus(0xFFFF);
	uint8_t IF = ReadBus(0xFF0F);
	uint8_t pending = IE & IF & 0x1F;

	if (!IME || pending == 0) return;

	for (int bit = 0; bit < 5; bit++) {
		if (pending & (1 << bit)) {
			IME = false;                       // disable further interrupts
			WriteBus(0xFF0F, IF & ~(1 << bit)); // clear the serviced bit
			halted = false;                    // wake up if HALTed

			// push current PC like a CALL
			SP--; WriteBus(SP, (PC >> 8) & 0xFF);
			SP--; WriteBus(SP, PC & 0xFF);

			static const uint16_t vectors[5] = { 0x40, 0x48, 0x50, 0x58, 0x60 };
			PC = vectors[bit];

			cycles += 20; // 5 M-cycles
			break; // only service one, lowest bit = highest priority
		}
	}
}

