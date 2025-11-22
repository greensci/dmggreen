#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>
#include <bitset>
#include "bus.h"
#include "cpu.h"
#include "instructions.h"
#include "ext_instructions.h"

RegisterPair AF, BC, DE, HL;
uint16_t SP;
uint16_t PC = 0x0000;
bool halted = false;
int cycles = 0x00;
uint8_t currentOP;
const char* OPCODES[0x100] = {
   nullptr
};
std::string logdata = "";
int zaza = 0;
int zazai = 0;
uint16_t tempPC = 0x00;

bool exportOps = false;

void ExecuteOpcode(uint8_t op) {
	currentOP = op;
	tempPC = PC - 1;
	uint8_t tempA, tempF, tempIF, tempIE;
	uint16_t tempBC, tempDE, tempHL, tempSP;
	zaza++;
	tempA = AF.hi;
	tempF = AF.lo;
	tempBC = BC.full;
	tempDE = DE.full;
	tempHL = HL.full;
	//tempIF = bus.IF;
	//tempIE = bus.IE;
	tempSP = SP;


	printf("%04X - 0x%02X - ", tempPC, op);
	switch (op) {
	case 0x00: nop(); break;
	case 0x01: ld_rr_d16(BC.full, ReadBus(PC), ReadBus(PC + 1)); break;
	case 0x02: ld_addr_r(BC.full, AF.hi); break;
	case 0x03: inc_rr(BC.full); break;
	case 0x04: inc_r(BC.hi); break;
	case 0x05: dec_r(BC.hi); break;
	case 0x06: ld_r_d8(BC.hi, ReadBus(PC)); break;
	case 0x07: rlca(); break;
	case 0x08: ld_a16_sp(ReadBus(PC), ReadBus(PC + 1)); break;
	case 0x09: add_hl_rr(BC.full); break;
	case 0x0A: ld_r_addr(AF.hi, BC.full); break;
	case 0x0B: dec_rr(BC.full); break;
	case 0x0C: inc_r(BC.lo); break;
	case 0x0D: dec_r(BC.lo); break;
	case 0x0E: ld_r_d8(BC.lo, ReadBus(PC)); break;
	case 0x0F: rrca(); break;



	case 0x10: /*stop*/ PC++; break;
	case 0x11: ld_rr_d16(DE.full, ReadBus(PC), ReadBus(PC + 1)); break;
	case 0x12: ld_addr_r(DE.full, AF.hi); break;
	case 0x13: inc_rr(DE.full); break;
	case 0x14: inc_r(DE.hi); break;
	case 0x15: dec_r(DE.hi); break;
	case 0x16: ld_r_d8(DE.hi, ReadBus(PC)); break;
	case 0x17: rla(); break;
	case 0x18: jr_n(ReadBus(PC)); break;
	case 0x19: add_hl_rr(DE.full); break;
	case 0x1A: ld_r_addr(AF.hi, DE.full); break;
	case 0x1B: dec_rr(DE.full); break;
	case 0x1C: inc_r(DE.lo); break;
	case 0x1D: dec_r(DE.lo); break;
	case 0x1E: ld_r_d8(DE.lo, ReadBus(PC)); break;
	case 0x1F: rra(); break;



	case 0x20: jr_f(ReadBus(PC), FLAG_Z, true); break;
	case 0x21: ld_rr_d16(HL.full, ReadBus(PC), ReadBus(PC + 1)); break;
	case 0x22: ld_addr_r(HL.full, AF.hi, 1); break;
	case 0x23: inc_rr(HL.full); break;
	case 0x24: inc_r(HL.hi); break;
	case 0x25: dec_r(HL.hi); break;
	case 0x26: ld_r_d8(HL.hi, ReadBus(PC)); break;
	case 0x27: daa(); break;
	case 0x28: jr_f(ReadBus(PC), FLAG_Z, false); break;
	case 0x29: add_hl_rr(HL.full); break;
	case 0x2A: ld_r_addr(AF.hi, HL.full, 1); break;
	case 0x2B: dec_rr(HL.full); break;
	case 0x2C: inc_r(HL.lo); break;
	case 0x2D: dec_r(HL.lo); break;
	case 0x2E: ld_r_d8(HL.lo, ReadBus(PC)); break;
	case 0x2F: cpl(); break;



	case 0x30: jr_f(ReadBus(PC), FLAG_C, true); break;
	case 0x31: ld_rr_d16(SP, ReadBus(PC), ReadBus(PC + 1)); break;
	case 0x32: ld_addr_r(HL.full, AF.hi, -1); break;
	case 0x33: inc_rr(SP); break;
	case 0x34: inc_addr(HL.full); break;
	case 0x35: dec_addr(HL.full); break;
	case 0x36: ld_addr_d8(HL.full, ReadBus(PC)); break;
	case 0x37: scf(); break;
	case 0x38: jr_f(ReadBus(PC), FLAG_C, false); break;
	case 0x39: add_hl_rr(SP); break;
	case 0x3A: ld_r_addr(AF.hi, HL.full, -1); break;
	case 0x3B: dec_rr(SP); break;
	case 0x3C: inc_r(AF.hi); break;
	case 0x3D: dec_r(AF.hi); break;
	case 0x3E: ld_r_d8(AF.hi, ReadBus(PC));  break;
	case 0x3F: ccf(); break;



	case 0x40: ld_r_r(BC.hi, BC.hi); break;
	case 0x41: ld_r_r(BC.hi, BC.lo); break;
	case 0x42: ld_r_r(BC.hi, DE.hi); break;
	case 0x43: ld_r_r(BC.hi, DE.lo); break;
	case 0x44: ld_r_r(BC.hi, HL.hi); break;
	case 0x45: ld_r_r(BC.hi, HL.lo); break;
	case 0x46: ld_r_addr(BC.hi, HL.full); break;
	case 0x47: ld_r_r(BC.hi, AF.hi); break;
	case 0x48: ld_r_r(BC.lo, BC.hi); break;
	case 0x49: ld_r_r(BC.lo, BC.lo); break;
	case 0x4A: ld_r_r(BC.lo, DE.hi); break;
	case 0x4B: ld_r_r(BC.lo, DE.lo); break;
	case 0x4C: ld_r_r(BC.lo, HL.hi); break;
	case 0x4D: ld_r_r(BC.lo, HL.lo); break;
	case 0x4E: ld_r_addr(BC.lo, HL.full); break;
	case 0x4F: ld_r_r(BC.lo, AF.hi); break;



	case 0x50: ld_r_r(DE.hi, BC.hi); break;
	case 0x51: ld_r_r(DE.hi, BC.lo); break;
	case 0x52: ld_r_r(DE.hi, DE.hi); break;
	case 0x53: ld_r_r(DE.hi, DE.lo); break;
	case 0x54: ld_r_r(DE.hi, HL.hi); break;
	case 0x55: ld_r_r(DE.hi, HL.lo); break;
	case 0x56: ld_r_addr(DE.hi, HL.full); break;
	case 0x57: ld_r_r(DE.hi, AF.hi); break;
	case 0x58: ld_r_r(DE.lo, BC.hi); break;
	case 0x59: ld_r_r(DE.lo, BC.lo); break;
	case 0x5A: ld_r_r(DE.lo, DE.hi); break;
	case 0x5B: ld_r_r(DE.lo, DE.lo); break;
	case 0x5C: ld_r_r(DE.lo, HL.hi); break;
	case 0x5D: ld_r_r(DE.lo, HL.lo); break;
	case 0x5E: ld_r_addr(DE.lo, HL.full); break;
	case 0x5F: ld_r_r(DE.lo, AF.hi); break;



	case 0x60: ld_r_r(HL.hi, BC.hi); break;
	case 0x61: ld_r_r(HL.hi, BC.lo); break;
	case 0x62: ld_r_r(HL.hi, DE.hi); break;
	case 0x63: ld_r_r(HL.hi, DE.lo); break;
	case 0x64: ld_r_r(HL.hi, HL.hi); break;
	case 0x65: ld_r_r(HL.hi, HL.lo); break;
	case 0x66: ld_r_addr(HL.hi, HL.full); break;
	case 0x67: ld_r_r(HL.hi, AF.hi); break;
	case 0x68: ld_r_r(HL.lo, BC.hi); break;
	case 0x69: ld_r_r(HL.lo, BC.lo); break;
	case 0x6A: ld_r_r(HL.lo, DE.hi); break;
	case 0x6B: ld_r_r(HL.lo, DE.lo); break;
	case 0x6C: ld_r_r(HL.lo, HL.hi); break;
	case 0x6D: ld_r_r(HL.lo, HL.lo); break;
	case 0x6E: ld_r_addr(HL.lo, HL.full); break;
	case 0x6F: ld_r_r(HL.lo, AF.hi); break;



	case 0x70: ld_addr_r(HL.full, BC.hi); break;
	case 0x71: ld_addr_r(HL.full, BC.lo); break;
	case 0x72: ld_addr_r(HL.full, DE.hi); break;
	case 0x73: ld_addr_r(HL.full, DE.lo); break;
	case 0x74: ld_addr_r(HL.full, HL.hi); break;
	case 0x75: ld_addr_r(HL.full, HL.lo); break;
	case 0x76: halt(); break;
	case 0x77: ld_addr_r(HL.full, AF.hi); break;
	case 0x78: ld_r_r(AF.hi, BC.hi); break;
	case 0x79: ld_r_r(AF.hi, BC.lo); break;
	case 0x7A: ld_r_r(AF.hi, DE.hi); break;
	case 0x7B: ld_r_r(AF.hi, DE.lo); break;
	case 0x7C: ld_r_r(AF.hi, HL.hi); break;
	case 0x7D: ld_r_r(AF.hi, HL.lo); break;
	case 0x7E: ld_r_addr(AF.hi, HL.full); break;
	case 0x7F: ld_r_r(AF.hi, AF.hi); break;



	case 0x80: add_r_r(AF.hi, BC.hi); break;
	case 0x81: add_r_r(AF.hi, BC.lo); break;
	case 0x82: add_r_r(AF.hi, DE.hi); break;
	case 0x83: add_r_r(AF.hi, DE.lo); break;
	case 0x84: add_r_r(AF.hi, HL.hi); break;
	case 0x85: add_r_r(AF.hi, HL.lo); break;
	case 0x86: add_r_addr(AF.hi, HL.full); break;
	case 0x87: add_r_r(AF.hi, AF.hi); break;
	case 0x88: adc_r_r(AF.hi, BC.hi); break;
	case 0x89: adc_r_r(AF.hi, BC.lo); break;
	case 0x8A: adc_r_r(AF.hi, DE.hi); break;
	case 0x8B: adc_r_r(AF.hi, DE.lo); break;
	case 0x8C: adc_r_r(AF.hi, HL.hi); break;
	case 0x8D: adc_r_r(AF.hi, HL.lo); break;
	case 0x8E: adc_r_addr(AF.hi, HL.full); break;
	case 0x8F: adc_r_r(AF.hi, AF.hi); break;



	case 0x90: sub_r_r(AF.hi, BC.hi); break;
	case 0x91: sub_r_r(AF.hi, BC.lo); break;
	case 0x92: sub_r_r(AF.hi, DE.hi); break;
	case 0x93: sub_r_r(AF.hi, DE.lo); break;
	case 0x94: sub_r_r(AF.hi, HL.hi); break;
	case 0x95: sub_r_r(AF.hi, HL.lo); break;
	case 0x96: sub_r_addr(AF.hi,HL.full); break;
	case 0x97: sub_r_r(AF.hi, AF.hi); break;
	case 0x98: sbc_r_r(AF.hi, BC.hi); break;
	case 0x99: sbc_r_r(AF.hi, BC.lo); break;
	case 0x9A: sbc_r_r(AF.hi, DE.hi); break;
	case 0x9B: sbc_r_r(AF.hi, DE.lo); break;
	case 0x9C: sbc_r_r(AF.hi, HL.hi); break;
	case 0x9D: sbc_r_r(AF.hi, HL.lo); break;
	case 0x9E: sbc_r_addr(AF.hi, HL.full); break;
	case 0x9F: sbc_r_r(AF.hi, AF.hi); break;



	case 0xA0: and_r_r(AF.hi, BC.hi); break;
	case 0xA1: and_r_r(AF.hi, BC.lo); break;
	case 0xA2: and_r_r(AF.hi, DE.hi); break;
	case 0xA3: and_r_r(AF.hi, DE.lo); break;
	case 0xA4: and_r_r(AF.hi, HL.hi); break;
	case 0xA5: and_r_r(AF.hi, HL.lo); break;
	case 0xA6: and_r_addr(AF.hi, HL.full); break;
	case 0xA7: and_r_r(AF.hi, AF.hi); break;
	case 0xA8: xor_r_r(AF.hi, BC.hi); break;
	case 0xA9: xor_r_r(AF.hi, BC.lo); break;
	case 0xAA: xor_r_r(AF.hi, DE.hi); break;
	case 0xAB: xor_r_r(AF.hi, DE.lo); break;
	case 0xAC: xor_r_r(AF.hi, HL.hi); break;
	case 0xAD: xor_r_r(AF.hi, HL.lo); break;
	case 0xAE: xor_r_addr(AF.hi, HL.full); break;
	case 0xAF: xor_r_r(AF.hi, AF.hi); break;



	case 0xB0: or_r_r(AF.hi, BC.hi); break;
	case 0xB1: or_r_r(AF.hi, BC.lo); break;
	case 0xB2: or_r_r(AF.hi, DE.hi); break;
	case 0xB3: or_r_r(AF.hi, DE.lo); break;
	case 0xB4: or_r_r(AF.hi, HL.hi); break;
	case 0xB5: or_r_r(AF.hi, HL.lo); break;
	case 0xB6: or_r_addr(AF.hi, HL.full); break;
	case 0xB7: or_r_r(AF.hi, AF.hi); break;
	case 0xB8: cp_r_r(AF.hi, BC.hi); break;
	case 0xB9: cp_r_r(AF.hi, BC.lo); break;
	case 0xBA: cp_r_r(AF.hi, DE.hi); break;
	case 0xBB: cp_r_r(AF.hi, DE.lo); break;
	case 0xBC: cp_r_r(AF.hi, HL.hi); break;
	case 0xBD: cp_r_r(AF.hi, HL.lo); break;
	case 0xBE: cp_r_addr(AF.hi, HL.full); break;
	case 0xBF: cp_r_r(AF.hi, AF.hi); break;



	case 0xC0: ret_f(FLAG_Z, true); break; // RET NZ
	case 0xC1: pop_rr(BC.full); break; // POP BC
	case 0xC2: jp_f(FLAG_Z, ReadBus(PC), ReadBus(PC + 1), true);  break; // JP NZ, a16
	case 0xC3: jp_a16(ReadBus(PC), ReadBus(PC + 1)); break; //JP a16
	case 0xC4: call_f_a16(FLAG_Z, ReadBus(PC), ReadBus(PC + 1), true); break; // CALL NZ, a16
	case 0xC5: push_rr(BC.full); break; // PUSH BC
	case 0xC6: add_r_r(AF.hi, ReadBus(PC)); PC++; break;
	case 0xC7: rst_addr(0x00); break;
	case 0xC8: ret_f(FLAG_Z, false); break; // RET Z
	case 0xC9: ret(); break; // RET
	case 0xCA: jp_f(FLAG_Z, ReadBus(PC), ReadBus(PC + 1), false);  break; // JP Z, a16
	case 0xCB: ExecuteExtInstruction(ReadBus(PC)); break; // CB PREFIX
	case 0xCC: call_f_a16(FLAG_Z, ReadBus(PC), ReadBus(PC + 1), false); break; // CALL NZ, a16
	case 0xCD: call_a16(ReadBus(PC), ReadBus(PC + 1)); break; // CALL a16
	case 0xCE: adc_r_r(AF.hi, ReadBus(PC)); PC++; break;
	case 0xCF: rst_addr(0x08); break;



	case 0xD0: ret_f(FLAG_C, true); break; // RET NC
	case 0xD1: pop_rr(DE.full); break; // POP DE
	case 0xD2: jp_f(FLAG_C, ReadBus(PC), ReadBus(PC + 1), true);  break; // JP NC, a16
	case 0xD3: /*nothing*/ break; //NOTHIGN
	case 0xD4: call_f_a16(FLAG_C, ReadBus(PC), ReadBus(PC + 1), true); break; // CALL NC, a16
	case 0xD5: push_rr(DE.full); break; // PUSH DE
	case 0xD6: sub_r_r(AF.hi, ReadBus(PC));  PC++; break; // ADD A, a8
	case 0xD7: rst_addr(0x10); break;
	case 0xD8: ret_f(FLAG_C, false); break;  // RET C
	case 0xD9: reti(); break; // RETI
	case 0xDA: jp_f(FLAG_C, ReadBus(PC), ReadBus(PC + 1), false); break; // JP C, a16
	case 0xDB: /*nothing*/ break; // nothing
	case 0xDC: call_f_a16(FLAG_C, ReadBus(PC), ReadBus(PC + 1), false); break; // CALL C, a16
	case 0xDD: /*nothing*/ break; // nothing
	case 0xDE: sbc_r_r(AF.hi, ReadBus(PC)); PC++; break;
	case 0xDF: rst_addr(0x18); break;



	case 0xE0: ldh_a8_r(ReadBus(PC), true); break; // LDH (a8),A
	case 0xE1: pop_rr(HL.full); break; // POP HL
	case 0xE2: ldh_a8_r(BC.lo); break; // LD (C), A
	case 0xE3: /*nothing*/ break;
	case 0xE4: /*nothing*/ break;
	case 0xE5: push_rr(HL.full); break; // PUSH HL
	case 0xE6: and_r_r(AF.hi, ReadBus(PC)); PC++; break;
	case 0xE7: rst_addr(0x20); break;// RST h20
	case 0xE8: add_sp_s8(static_cast<int8_t>(ReadBus(PC))); break; // ADD SP, a8
	case 0xE9: jp_hl(); break; // JP hl
	case 0xEA: ld_a16_a(ReadBus(PC), ReadBus(PC + 1)); break; // LD a16, A
	case 0xEB: /*nothing*/ break; // nothing
	case 0xEC: /*nothing*/ break; // nothing
	case 0xED: /*nothing*/ break; // nothing
	case 0xEE: xor_r_r(AF.hi, ReadBus(PC)); PC++; break;
	case 0xEF: rst_addr(0x28); break;



	case 0xF0: ldh_r_a8(ReadBus(PC), true); break;
	case 0xF1: pop_rr(AF.full); AF.lo &= 0xf0; break; // POP AF
	case 0xF2: ldh_r_a8(BC.lo); break; // LD A,(C)
	case 0xF3: di(); /* disable IME after next instruction*/ break; // DI
	case 0xF4: /*nothing*/ break; // nothing
	case 0xF5: push_rr(AF.full); break; // PUSH AF
	case 0xF6: or_r_r(AF.hi, ReadBus(PC)); PC++; break;
	case 0xF7: rst_addr(0x30); break; // RST h30
	case 0xF8: ld_hl_sp_s8(ReadBus(PC)); break;
	case 0xF9: ld_sp_hl(); break; // LD SP,HL
	case 0xFA: ld_a_a16(ReadBus(PC), ReadBus(PC + 1)); break; // LD A, a16
	case 0xFB: ei(); /* enable IME after next instruction*/ break; // EI
	case 0xFC: /*nothing*/ break; // nothing
	case 0xFD: /*nothing*/ break; // nothing
	case 0xFE: cp_r_r(AF.hi, ReadBus(PC)); PC++; break;
	case 0xFF: rst_addr(0x38); break;

		



	default: printf("UNKNOWN OPCODE"); break;
	}

	/*std::ofstream configFile("cpulogs.log", std::ios::app);  // std::ios::app for append mode

	configFile << "A:" + byteToHexString(tempA) + " ";
	configFile << "F:" + byteToHexString(tempF) + " ";
	configFile << "B:" + byteToHexString(tempBC >> 8) + " ";
	configFile << "C:" + byteToHexString(tempBC & 0xFF) + " ";
	configFile << "D:" + byteToHexString(tempDE >> 8) + " ";
	configFile << "E:" + byteToHexString(tempDE & 0xFF) + " ";
	configFile << "H:" + byteToHexString(tempHL >> 8) + " ";
	configFile << "L:" + byteToHexString(tempHL & 0xFF) + " ";
	configFile << "SP:" + wordToHexString(tempSP) + " ";
	configFile << "PC:" + wordToHexString(tempPC) + " ";
	configFile << "PCMEM:" + byteToHexString(ReadBus(tempPC)) + ",";
	configFile << byteToHexString(ReadBus(tempPC + 1)) + ",";
	configFile << byteToHexString(ReadBus(tempPC + 2)) + ",";
	configFile << byteToHexString(ReadBus(tempPC + 3)) << std::endl;

	
	configFile.close();*/
	if (exportOps) {
		logdata += "A:" + byteToHexString(tempA) + " ";
		logdata += "F:" + byteToHexString(tempF) + " ";
		logdata += "B:" + byteToHexString(tempBC >> 8) + " ";
		logdata += "C:" + byteToHexString(tempBC & 0xFF) + " ";
		logdata += "D:" + byteToHexString(tempDE >> 8) + " ";
		logdata += "E:" + byteToHexString(tempDE & 0xFF) + " ";
		logdata += "H:" + byteToHexString(tempHL >> 8) + " ";
		logdata += "L:" + byteToHexString(tempHL & 0xFF) + " ";
		logdata += "SP:" + wordToHexString(tempSP) + " ";
		logdata += "PC:" + wordToHexString(tempPC) + " ";
		logdata += "PCMEM:" + byteToHexString(ReadBus(tempPC)) + ",";
		logdata += byteToHexString(ReadBus(tempPC + 1)) + ",";
		logdata += byteToHexString(ReadBus(tempPC + 2)) + ",";
		logdata += byteToHexString(ReadBus(tempPC + 3)) + "\n";
		if (zaza == (zazai + 3000)) {
			std::ofstream configFile("cpulogs.log");
			configFile << logdata << std::endl;
			configFile.close();
			zazai = zaza;
		}
	}
	
	if ((ReadBus(0xDFF1) == 0xAB) && tempPC == 0xC092) {
		tempSP;
		uint8_t lo = ReadBus(0xDFF1);
		uint8_t hi = ReadBus(0xDFF2);
	}

	printf(" | A: %02X F: %02X (b%s) BC: %04X DE: %04X HL: %04X SP: %04X\n", AF.hi, AF.lo, std::bitset<8>(AF.lo).to_string().c_str(), BC.full, DE.full, HL.full, SP);
	
}
std::string byteToHexString(uint8_t value) {
	char buffer[3];
	snprintf(buffer, sizeof(buffer), "%02X", value);
	return buffer;
}

std::string wordToHexString(uint16_t value) {
	char buffer[5];
	snprintf(buffer, sizeof(buffer), "%04X", value);
	return buffer;
}