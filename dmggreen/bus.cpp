#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>
#include "bus.h"
#include "cpu.h"
#include "instructions.h"

uint8_t memory[0xffff];

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