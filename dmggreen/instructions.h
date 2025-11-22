#pragma once

#ifndef INSTS_H
#define INSTS_H


void setFlag(Flag flag, bool value);
bool getFlag(Flag flag);

void halt();
void nop();
void ld_rr_d16(uint16_t& reg, uint8_t n1, uint8_t n2);
void ld_addr_r(uint16_t& rr, uint8_t r, int ifthing = 0);
void inc_rr(uint16_t& rr);
void inc_r(uint8_t& r);
void dec_r(uint8_t& r);
void ld_r_d8(uint8_t& r, uint8_t n);
void rlca();
void ld_a16_sp(uint8_t lo, uint8_t hi);
void add_hl_rr(uint16_t rr);
void ld_r_addr(uint8_t& r, uint16_t& rr, int ifthing = 0);
void dec_rr(uint16_t& rr);
void rrca();
void rla();
void jr_n(int8_t offset);
void rra();
void jr_f(int8_t offset, Flag flag, bool ifNot);
void daa();
void cpl();
void inc_addr(uint16_t address);
void dec_addr(uint16_t address);
void ld_addr_d8(uint16_t& address, uint8_t n);
void scf();
void ccf();
void ld_r_r(uint8_t& r, uint8_t n);
void add_r_r(uint8_t& reg1, uint8_t reg2);
void add_r_addr(uint8_t& reg1, uint16_t addr);
void adc_r_r(uint8_t& reg1, uint8_t reg2);
void adc_r_addr(uint8_t& reg1, uint16_t addr);
void sub_r_r(uint8_t& reg1, uint8_t reg2);
void sub_r_addr(uint8_t& reg1, uint16_t addr);
void sbc_r_r(uint8_t& reg1, uint8_t reg2);
void sbc_r_addr(uint8_t& reg1, uint16_t addr);
void and_r_r(uint8_t& reg1, uint8_t reg2);
void and_r_addr(uint8_t& reg1, uint16_t addr);
void xor_r_r(uint8_t& reg1, uint8_t reg2);
void xor_r_addr(uint8_t& reg1, uint16_t addr);
void or_r_r(uint8_t& reg1, uint8_t reg2);
void or_r_addr(uint8_t& reg1, uint16_t addr);
void cp_r_r(uint8_t& reg1, uint8_t reg2);
void cp_r_addr(uint8_t& reg1, uint16_t addr);
void ret_f(Flag flag, bool ifNot);
void pop_rr(uint16_t& rr);
void jp_f(Flag flag, uint8_t low, uint8_t high, bool ifNot);
void jp_a16(uint8_t low, uint8_t high);
void call_f_a16(Flag flag, uint8_t low, uint8_t high, bool ifNot);
void push_rr(uint16_t rr);
void rst_addr(uint8_t address);
void ret();
void call_a16(uint8_t low, uint8_t high);
void reti(); //WIPPPP
void jp_hl();
void ldh_a8_r(uint8_t n, bool twobyte = false);
void add_sp_s8(int8_t offset);
void ld_a16_a(uint8_t low, uint8_t high);
void ldh_r_a8(uint8_t n, bool twobyte = false);
void di(); //WIPPPP
void ld_hl_sp_s8(int8_t offset);
void ld_sp_hl();
void ld_a_a16(uint8_t low, uint8_t high);
void ei(); //WIPPPP

#endif
