#pragma once

#ifndef BUS_H
#define BUS_H

extern uint8_t memory[0xffff];
uint8_t ReadBus(uint16_t addr);
void WriteBus(uint16_t addr, uint8_t val);

#endif
