#pragma once

#ifndef BUS_H
#define BUS_H

extern uint8_t memory[0x10000];
uint8_t ReadBus(uint16_t addr);
void WriteBus(uint16_t addr, uint8_t val);
void dbg_update();
void dbg_print();
void CheckInterrupts();

#endif
