#pragma once

#ifndef EXTINSTS_H
#define EXTINSTS_H

extern const uint8_t extendedInstructionTicks[256];


void ExecuteExtInstruction(uint8_t opcode);
void rlc_r(uint8_t& reg);
void rlc_rr(uint16_t rr);
void rrc_r(uint8_t& reg);
void rrc_rr(uint16_t rr);
void rl_r(uint8_t& reg);
void rl_rr(uint16_t rr);
void rr_r(uint8_t& reg);
void rr_rr(uint16_t rr);
void sla_r(uint8_t& reg);
void sla_rr(uint16_t rr);
void sra_r(uint8_t& reg);
void sra_rr(uint16_t rr);
void swap_r(uint8_t& reg);
void swap_rr(uint16_t rr);
void srl_r(uint8_t& reg);
void srl_rr(uint16_t rr);
void bit_n_r(uint8_t bit, uint8_t& reg);
void bit_n_rr(uint8_t bit, uint16_t rr);
void res_n_r(uint8_t bit, uint8_t& reg);
void res_n_rr(uint8_t bit, uint16_t rr);
void set_n_r(uint8_t bit, uint8_t& reg);
void set_n_rr(uint8_t bit, uint16_t rr);

#endif