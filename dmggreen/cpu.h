#ifndef CPU_H
#define CPU_H

#include <cstdint>

// Register Pair union for 16-bit and 8-bit access
union RegisterPair {
    struct {
        uint8_t lo;
        uint8_t hi;
    };
    uint16_t full;

    RegisterPair() : full(0) {}
};

// Flag bits for the F register
enum Flag {
    FLAG_Z = 1 << 7, // Zero
    FLAG_N = 1 << 6, // Subtract
    FLAG_H = 1 << 5, // Half Carry
    FLAG_C = 1 << 4  // Carry
}; // flag values based on the F register

// External declarations for global registers
extern RegisterPair AF, BC, DE, HL;
extern uint16_t SP;
extern uint16_t PC;
extern uint8_t currentOP;
extern bool halted;
extern int cycles;
extern std::string logdata;

extern bool exportOps;
extern uint16_t tempPC;

// Opcode execution
void ExecuteOpcode(uint8_t op);
std::string byteToHexString(uint8_t value);
std::string wordToHexString(uint16_t value);

#endif // CPU_H