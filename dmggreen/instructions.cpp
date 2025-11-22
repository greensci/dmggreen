#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>
#include "bus.h"
#include "cpu.h"
#include "ext_instructions.h"


const char* flagNames[] = { "Z", "N", "H", "C" };


void setFlag(Flag flag, bool value) {
	if (value)
		AF.lo |= flag;
	else
		AF.lo &= ~flag;
}
bool getFlag(Flag flag) {
	return AF.lo & flag;
}


void halt() {
	halted = true;
	printf("HALT");
}
void nop() {
	cycles += 4;
	printf("NOP");
}
void ld_rr_d16(uint16_t& reg, uint8_t n1, uint8_t n2) {
	reg = (n2 << 8) | n1;
	cycles += 12;
	PC += 2;
	printf("LD RR, $%02X%02X", n2, n1);
}
void ld_addr_r(uint16_t& rr, uint8_t r, int ifthing = 0) {

	//std::cout << address << " ld addr, rr";
	WriteBus(rr, r); // AF.hi = A
	if (ifthing == -1) {
		rr--;
	}
	else if (ifthing == 1) {
		rr++;
	}
	cycles += 8;
	printf("LD ($%04X), R", rr);
}
void inc_rr(uint16_t& rr) {
	rr += 1;
	cycles += 8;
	printf("INC RR");
}
void inc_r(uint8_t& r) {

	uint8_t old = r; // save original value
	uint8_t result = r + 1;

	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, (old & 0x0F) == 0x0F);

	r = result;
	cycles += 4;
	printf("INC R");
}
void dec_r(uint8_t& r) {

	uint8_t old = r; // save original value
	uint8_t result = r - 1;

	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, !(old & 0x0F));

	r = result;
	cycles += 4;
	printf("DEC R");
}
void ld_r_d8(uint8_t& r, uint8_t n) {

	r = n;
	cycles += 4;
	PC++;
	printf("LD R, 0x%02X", n);
}
void rlca() {
	uint8_t& r = AF.hi;
	uint8_t bit7 = (r & 0x80) >> 7; // Get bit 7

	r = (r << 1) | bit7;

	setFlag(FLAG_Z, false); // Always cleared
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, bit7);
	cycles += 4;
	printf("RLCA");
}
void ld_a16_sp(uint8_t lo, uint8_t hi) {
	uint16_t addr = (hi << 8) | lo;
	WriteBus(addr, SP & 0xFF);
	WriteBus(addr + 1, (SP >> 8) & 0xFF);
	cycles += 20;
	PC += 2;
	printf("LD ($%04X), SP", addr);

}
void add_hl_rr(uint16_t rr) {
	uint32_t result = HL.full + rr;

	setFlag(FLAG_C, result > 0xffff);
	setFlag(FLAG_H, ((HL.full & 0x0fff) + (rr & 0x0fff)) > 0x0fff);

	HL.full = (uint16_t)result;

	setFlag(FLAG_N, false);

	cycles += 8;
	printf("ADD HL, RR");
}
void ld_r_addr(uint8_t& r, uint16_t& rr, int ifthing = 0) {

	//std::cout << address << " ld addr, rr";
	r = ReadBus(rr); // AF.hi = A

	if (ifthing == -1) {
		rr--;
	}
	else if (ifthing == 1) {
		rr++;
	}

	cycles += 8;
	printf("LD R, ($%04X)", rr);
}
void dec_rr(uint16_t& rr) {

	rr -= 1;
	cycles += 8;
	printf("DEC RR");
}
void rrca() {
	uint8_t& r = AF.hi;
	uint8_t bit0 = r & 0x01; // Save bit 0

	r = (r >> 1) | (bit0 << 7); // Rotate right, bit 0 goes to bit 7

	setFlag(FLAG_C, bit0);  // Set Carry flag to old bit 0
	setFlag(FLAG_Z, false); // Z is always cleared
	setFlag(FLAG_N, false); // N is always cleared
	setFlag(FLAG_H, false); // H is always cleared
	cycles += 4;
	printf("RRCA");
}
void rla() {
	uint8_t& A = AF.hi;
	uint8_t bit7 = (A >> 7) & 0x01;  // Get bit 7 (highest bit of A)
	uint8_t carry = getFlag(FLAG_C); // Get current Carry flag value

	A = (A << 1) | carry; // Shift A left and place carry in bit 0
	setFlag(FLAG_C, bit7); // Set Carry flag to bit 7 of A

	setFlag(FLAG_Z, false); // Always cleared
	setFlag(FLAG_N, false); // Always cleared  
	setFlag(FLAG_H, false); // Always cleared
	cycles += 4;
	printf("RLA"); // Fixed instruction name
}
void jr_n(int8_t offset) { // JR S8 : Adds the given value to PC counter
	PC++;
	PC += offset;
	cycles += 12;   // JR takes 3 cycles when jumped
	printf("JR %+d", offset); // ignore
}
void rra() {
	uint8_t& r = AF.hi;
	uint8_t bit0 = r & 0x01;  // Get bit 0 (lowest bit of A)
	uint8_t carry = getFlag(FLAG_C); // Get current Carry flag value

	r = (r >> 1) | (carry << 7); // Shift A right and place carry in bit 7
	setFlag(FLAG_C, bit0); // Set Carry flag to bit 0 of A

	setFlag(FLAG_Z, false); // Always cleared
	setFlag(FLAG_N, false); // Always cleared
	setFlag(FLAG_H, false); // Always cleared
	cycles += 4;
	printf("RRA");
}
void jr_f(int8_t offset, Flag flag, bool ifNot) {

	bool conditionMet = getFlag(flag);
	if (ifNot) conditionMet = !conditionMet;
	PC++;

	if (conditionMet) {
		PC += offset;  // Apply relative offset
		cycles += 12;   // 12 cycles total for taken jump
	}
	else {
		cycles += 8;   // 8 cycles for not taken
	}

	printf("JR %cF, %02X", ifNot ? 'N' : ' ', offset);
}
void daa() {
	uint8_t& A = AF.hi;
	bool N = getFlag(FLAG_N);
	bool H = getFlag(FLAG_H);
	bool C = getFlag(FLAG_C);

	uint8_t correction = 0;
	bool new_carry = C;  // track carry flag

	if (!N) { // after addition
		if (C || A > 0x99) {
			correction |= 0x60;
			new_carry = true;
		}
		if (H || (A & 0x0F) > 0x09) {
			correction |= 0x06;
		}
		A += correction;
	}
	else { // after subtraction
		if (C) {
			correction |= 0x60;
			new_carry = true;
		}
		if (H) {
			correction |= 0x06;
		}
		A -= correction;
		// carry flag is noT affected by subtraction in daa jj
	}

	setFlag(FLAG_Z, A == 0);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, new_carry);

	cycles += 4;
	printf("DAA");
}
void cpl() {
	AF.hi = ~AF.hi;         // A = NOOOT A!!!!!!!!
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, true);
	cycles += 4;
	printf("CPL");
}
void inc_addr(uint16_t address) {
	// Read from memory at the given address
	uint8_t old_value = ReadBus(address);
	uint8_t result = old_value + 1;

	// Write back to memory
	WriteBus(address, result);

	// Set flags
	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, (old_value & 0x0F) == 0x0F);

	cycles += 12;
	printf("INC ($%04X)\n", address);
}
void dec_addr(uint16_t address) {
	// Read from memory at the given address
	uint8_t old_value = ReadBus(address);
	uint8_t result = old_value - 1;

	// Write back to memory
	WriteBus(address, result);

	// Set flags
	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, (old_value & 0x0F) == 0x0F);

	cycles += 12;
	printf("DEC ($%04X)\n", address);
}
void ld_addr_d8(uint16_t& address, uint8_t n) {

	// Write back to memory
	WriteBus(address, n);

	cycles += 12;
	PC++;
	printf("LD ($%04X), 0x%02X", address, n);
}
void scf() {
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, true);
	// Z flag is unaffected
	cycles += 4;
	printf("SCF");
}
void ccf() {
	bool current_carry = getFlag(FLAG_C);
	setFlag(FLAG_C, !current_carry);  // Toggle carry flag
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	// Z flag is unaffected
	cycles += 8;
	printf("CCF");
}
void ld_r_r(uint8_t& r, uint8_t n) {
	r = n;
	cycles += 4;
	printf("LD R, R");
}
void add_r_r(uint8_t& reg1, uint8_t reg2) {
	uint16_t result = reg1 + reg2;

	// Set flags
	setFlag(FLAG_Z, (result & 0xFF) == 0);  // Check 8-bit result
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, (reg1 & 0x0F) + (reg2 & 0x0F) > 0x0F);
	setFlag(FLAG_C, result > 0xFF);

	reg1 = static_cast<uint8_t>(result);
	cycles += 4;
	printf("ADD A, R");
}
void add_r_addr(uint8_t& reg1, uint16_t addr) {
	uint8_t value = ReadBus(addr);  // Read the value from memory
	uint16_t result = reg1 + value;

	// Set flags
	setFlag(FLAG_Z, (result & 0xFF) == 0);
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, (reg1 & 0x0F) + (value & 0x0F) > 0x0F);  // Use the value, not the address
	setFlag(FLAG_C, result > 0xFF);

	reg1 = static_cast<uint8_t>(result);
	cycles += 8;
	printf("ADD A, ($%04X)", addr);
}
void adc_r_r(uint8_t& reg1, uint8_t reg2) {
	int carry = getFlag(FLAG_C) ? 1 : 0;
	int result = reg1 + reg2 + carry;

	setFlag(FLAG_Z, !(int8_t)result);
	setFlag(FLAG_C, result > 0xff);
	setFlag(FLAG_H, ((reg1 & 0x0F) + (reg2 & 0x0f) + carry) > 0x0F);
	setFlag(FLAG_N, false);

	reg1 = (int8_t)(result & 0xff);
	cycles += 4;


	printf("ADC A, R");
}
void adc_r_addr(uint8_t& reg1, uint16_t addr) {
	uint8_t carry = getFlag(FLAG_C) ? 1 : 0;
	uint8_t value = ReadBus(addr);  // Read once and cache
	uint16_t result = reg1 + value + carry;
	reg1 = static_cast<uint8_t>(result);
	// Set flags
	setFlag(FLAG_H, ((reg1 & 0x0F) + (value & 0x0F) + carry) > 0x0F);
	setFlag(FLAG_C, result > 0xFF);


	setFlag(FLAG_Z, reg1 == 0);
	setFlag(FLAG_N, false);
	cycles += 8;
	printf("ADC A, ($%04X)", addr);
}
void sub_r_r(uint8_t& reg1, uint8_t reg2) {
	uint8_t old_reg1 = reg1;
	uint8_t result = reg1 - reg2;

	// Set flags
	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, (old_reg1 & 0x0F) < (reg2 & 0x0F));  // Half-carry: borrow from bit 4
	setFlag(FLAG_C, old_reg1 < reg2);                   // Carry: borrow needed

	reg1 = result;
	cycles += 4;
	printf("SUB A, R");
}
void sub_r_addr(uint8_t& reg1, uint16_t addr) {
	uint8_t value = ReadBus(addr);
	uint8_t old_reg1 = reg1;
	uint8_t result = old_reg1 - value;

	// Set flags
	setFlag(FLAG_Z, result == 0);
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, (old_reg1 & 0x0F) < (value & 0x0F));  // Half-carry: borrow from bit 4
	setFlag(FLAG_C, old_reg1 < value);                    // Carry: borrow needed

	reg1 = result;
	cycles += 8;
	printf("SUB A, ($%04X)\n", addr);
}
void sbc_r_r(uint8_t& reg1, uint8_t reg2) {
	uint8_t a = reg1;
	uint8_t b = reg2;
	uint8_t carry = getFlag(FLAG_C) ? 1 : 0;
	uint16_t result = a - b - carry;

	// Set flags
	setFlag(FLAG_H, (a & 0x0F) < ((b & 0x0F) + carry));  // Half-carry: borrow from bit 4
	setFlag(FLAG_C, a < (b + carry));                    // Carry: borrow needed

	reg1 = static_cast<uint8_t>(result);
	setFlag(FLAG_Z, reg1 == 0);
	setFlag(FLAG_N, true);
	cycles += 4;
	printf("SBC A, R");
}
void sbc_r_addr(uint8_t& reg1, uint16_t addr) {
	uint8_t a = reg1;
	uint8_t b = ReadBus(addr);  // Read from memory
	uint8_t carry = getFlag(FLAG_C) ? 1 : 0;
	uint16_t result = a - b - carry;

	// Set flags
	setFlag(FLAG_H, (a & 0x0F) < ((b & 0x0F) + carry));  // Half-carry: borrow from bit 4
	setFlag(FLAG_C, a < (b + carry));                    // Carry: borrow needed

	reg1 = static_cast<uint8_t>(result);
	setFlag(FLAG_Z, reg1 == 0);
	setFlag(FLAG_N, true);
	cycles += 8;
	printf("SBC A, ($%04X)", addr);
}
void and_r_r(uint8_t& reg1, uint8_t reg2) {
	reg1 &= reg2;
	setFlag(FLAG_Z, (reg1 == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, true);
	setFlag(FLAG_C, false);
	cycles += 4;
	printf("AND A, R");
}
void and_r_addr(uint8_t& reg1, uint16_t addr) {
	uint8_t value = ReadBus(addr);
	reg1 &= value;

	setFlag(FLAG_Z, (reg1 == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, true);
	setFlag(FLAG_C, false);
	cycles += 8;
	printf("AND A, ($%04X)", addr);
}
void xor_r_r(uint8_t& reg1, uint8_t reg2) {
	reg1 ^= reg2;


	setFlag(FLAG_Z, (reg1 == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, false);
	cycles += 4;
	printf("XOR A, R");
}
void xor_r_addr(uint8_t& reg1, uint16_t addr) {
	reg1 ^= ReadBus(addr);

	setFlag(FLAG_Z, (reg1 == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, false);
	cycles += 8;
	printf("XOR A, ($%04X)", addr);
}
void or_r_r(uint8_t& reg1, uint8_t reg2) {
	reg1 |= reg2;

	setFlag(FLAG_Z, (reg1 == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, false);
	cycles += 4;
	printf("OR A, R");
}
void or_r_addr(uint8_t& reg1, uint16_t addr) {
	reg1 |= ReadBus(addr);

	setFlag(FLAG_Z, (reg1 == 0));
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
	setFlag(FLAG_C, false);
	cycles += 8;
	printf("OR A, ($%04X)", addr);
}

void cp_r_r(uint8_t& reg1, uint8_t reg2) {
	uint8_t a = reg1;  // Original value
	uint8_t b = reg2;

	// Calculate flags based on subtraction (but don't store result)
	setFlag(FLAG_Z, a == b);
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, (a & 0x0F) < (b & 0x0F));  // Half-carry: borrow from bit 4
	setFlag(FLAG_C, a < b);                    // Carry: borrow needed

	cycles += 4;
	printf("CP A, R");
}
void cp_r_addr(uint8_t& reg1, uint16_t addr) {
	uint8_t value = ReadBus(addr);  // Read from memory
	uint8_t a = reg1;               // Original register value

	// Set flags based on comparison (A - value)
	setFlag(FLAG_Z, a == value);
	setFlag(FLAG_N, true);
	setFlag(FLAG_H, (a & 0x0F) < (value & 0x0F));  // Half-carry: borrow from bit 4
	setFlag(FLAG_C, a < value);                    // Carry: borrow needed

	cycles += 8;
	printf("CP A, ($%04X)", addr);
}
void ret_f(Flag flag, bool ifNot) {
	cycles += 8;
	bool conditionMet = getFlag(flag);
	if (ifNot) conditionMet = !conditionMet;

	if (conditionMet) {
		uint8_t low = ReadBus(SP++);
		uint8_t high = ReadBus(SP++);
		PC = (high << 8) | low;
		cycles += 12;  // Total 20
	}
	printf("RET %c%s\n", ifNot ? 'N' : ' ', flagNames[flag]);
}
void pop_rr(uint16_t& rr) {
	uint8_t low = ReadBus(SP++);   // Read low byte and increment
	uint8_t high = ReadBus(SP++);  // Read high byte and increment
	rr = (high << 8) | low;
	cycles += 12;
	printf("POP RR");
}
void jp_f(Flag flag, uint8_t low, uint8_t high, bool ifNot) {
	bool conditionMet = getFlag(flag);
	uint16_t addr = (high << 8) | low;

	if (ifNot) conditionMet = !conditionMet;

	if (conditionMet) {
		PC = addr;  // Absolute jump
		cycles += 16;  // Total cycles for taken jump
	}
	else {
		cycles += 12;  // Cycles for not taken
		PC += 2;
	}
	printf("JP %cF, $%04X", ifNot ? 'N' : ' ', addr);
}
void jp_a16(uint8_t low, uint8_t high) {
	PC = (high << 8) | low;
	cycles += 16;
	printf("JP $%04X", (high << 8) | low);
}
void call_f_a16(Flag flag, uint8_t low, uint8_t high, bool ifNot) {
	bool conditionMet = getFlag(flag);
	uint16_t addr = (high << 8) | low;
	PC += 2;

	if (ifNot) conditionMet = !conditionMet;

	cycles += 12;
	if (conditionMet) {
		SP--;
		WriteBus(SP, (PC >> 8) & 0xFF);
		SP--;
		WriteBus(SP, PC & 0xFF);
		PC = addr;
		cycles += 12;
	}

	printf("CALL %c%s, $%04X", ifNot ? 'N' : ' ', flagNames[flag], addr);
}
void push_rr(uint16_t rr) {
	WriteBus(--SP, (rr >> 8) & 0xFF);   // Pre-decrement
	WriteBus(--SP, rr & 0xFF);          // Pre-decrement
	cycles += 16;
	printf("PUSH RR");
}
void rst_addr(uint8_t address) {
	// Push return address onto stack (current PC points to next instruction)
	SP--;
	WriteBus(SP, (PC >> 8) & 0xFF);  // Push high byte
	SP--;
	WriteBus(SP, PC & 0xFF);         // Push low byte

	// Jump to restart address (one of: 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38)
	PC = address;

	cycles += 16;
	printf("RST $%02X", address);
}
void ret() {
	uint8_t low = ReadBus(SP++);
	uint8_t high = ReadBus(SP++);
	PC = (high << 8) | low;
	cycles += 16;
	printf("RET");
}
void call_a16(uint8_t low, uint8_t high) {
	uint16_t addr = (high << 8) | low;
	uint16_t return_addr = PC + 2;  // CALL is 3 bytes

	SP--;
	WriteBus(SP, (return_addr >> 8) & 0xFF);
	SP--;
	WriteBus(SP, return_addr & 0xFF);

	PC = addr;
	cycles += 24;
	printf("CALL $%04X", addr);
}
void reti() {
	printf("RETI (WIP)");
}
void jp_hl() {
	PC = HL.full;  // Jump to address in HL
	cycles += 4;   // JP HL takes 4 clock cycles (1 machine cycle)
	printf("JP HL");
}
void ldh_a8_r(uint8_t n, bool twobyte = false) {
	uint16_t address = 0xFF00 + n;  // Calculate address in HRAM
	WriteBus(address, AF.hi);  // Write A to (0xFF00 + n)

	cycles += 8;
	if (twobyte) {
		cycles += 4;
		PC++;
	}
	printf("LD (%04X), R", n);
}
void add_sp_s8(int8_t offset) {

	// Calculate result (use 32-bit to detect carry)
	uint32_t result = SP + offset;

	// Set flags
	setFlag(FLAG_Z, false);        // Always 0
	setFlag(FLAG_N, false);        // Always 0  
	setFlag(FLAG_H, ((SP ^ offset ^ (result & 0xFFFF)) & 0x10) != 0);  // Half-carry from bit 3
	setFlag(FLAG_C, ((SP ^ offset ^ (result & 0xFFFF)) & 0x100) != 0); // Carry from bit 7

	SP = result & 0xFFFF;  // Store 16-bit result
	PC++;

	cycles += 16;
	printf("ADD SP, %+d", offset);
}
void ld_a16_a(uint8_t low, uint8_t high) {
	// Read 16-bit address (little-endian)
	uint16_t address = (high << 8) | low;

	// Write A to the memory address
	WriteBus(address, AF.hi);

	cycles += 16;
	PC += 2;
	printf("LD ($%04X), A", address);
}
void ldh_r_a8(uint8_t n, bool twobyte = false) {
	uint16_t address = 0xFF00 + n;  // Calculate address in HRAM
	AF.hi = ReadBus(address);  // Read from (0xFF00 + n) into A

	cycles += 8;
	if (twobyte) {
		cycles += 4;
		PC++;
	}
	printf("LD A, ($FF00+%02X)", n);
}
void di() {

}
void ld_hl_sp_s8(int8_t offset) {

	// Calculate result (use 32-bit to detect carry)
	uint32_t result = SP + offset;

	// Set flags (same as ADD SP, s8)
	setFlag(FLAG_Z, false);        // Always 0
	setFlag(FLAG_N, false);        // Always 0  
	setFlag(FLAG_H, ((SP ^ offset ^ (result & 0xFFFF)) & 0x10) != 0);  // Half-carry from bit 3
	setFlag(FLAG_C, ((SP ^ offset ^ (result & 0xFFFF)) & 0x100) != 0); // Carry from bit 7

	HL.full = result & 0xFFFF;  // Store 16-bit result in HL
	PC++;

	cycles += 12;
	printf("LD HL, SP+%+d", offset);
}
void ld_sp_hl() {
	SP = HL.full;  // Direct copy
	cycles += 8;
	printf("LD SP, HL");
}
void ld_a_a16(uint8_t low, uint8_t high) {
	// Read 16-bit address (little-endian)
	uint16_t address = (high << 8) | low;

	// Read from memory into A
	AF.hi = ReadBus(address);

	cycles += 16;
	PC += 2;
	printf("LD A, ($%04X)", address);
}
void ei() {

}