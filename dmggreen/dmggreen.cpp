// dmggreen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>
#include "bus.h"
#include "cpu.h"
#include "instructions.h"








int main(int argc, char* argv[])
{

	AF.hi = 0x01;
	AF.lo = 0xB0;
	BC.hi = 0x00;
	BC.lo = 0x13;
	DE.hi = 0x00;
	DE.lo = 0xD8;
	HL.hi = 0x01;
	HL.lo = 0x4D;
	SP = 0xFFFE;
	PC = 0x100;
	exportOps = true;
#ifdef DEBUG
	exportOps = false;

#endif // DEBUG

		
	

    if (argc > 1) {
		std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
		if (file.is_open())
		{

			std::cout << "[INFO] Loading ROM..\n";
			// get size of file and allocate a buffer to hold the contents
			std::streampos size = file.tellg();
			char* buffer = new char[size];




			// go back to the beginning of the file and fill the buffer
			file.seekg(0, std::ios::beg);
			file.read(buffer, size);
			file.close();
			if (size > 32768) {
				std::cout << "[INFO] ROM size is higher than 32768 bytes (32KB)! ROM will continue executing if it has MBC.\n";
				//size = 0x8000;
			}
			




			// load the rom into the gb's memory, since its going to rom_data and not main memory, it wont start at 0x0100 (256 bytes)
			int logocount = 0;
			int titlecount = 0;
			std::string titleting = "sex";
			bool logomatches = true;
			int size_filter = size;
			if (size_filter > 32768)
				size_filter = 0x7FFF;


			for (int i = 0; i < size_filter; ++i)
			{
			
					
					
				
				
					memory[i] = buffer[i];
				

			}

			

			
			std::cout << "[INFO] ROM '" << titleting << "' Loaded!\n\n";



			// clean the buffer
			delete[] buffer;
			
		}
    }
	
	std::ofstream configFile("cpulogs.log");



	configFile << "";

	configFile.close();
	WriteBus(0xDFFD, 0x00);
    
    while(!halted) {
		uint8_t opcode = ReadBus(PC);
        PC++;
        ExecuteOpcode(opcode);
    }
    std::cout << "halted!\n";
    printf("BC: 0x%04X\n", BC.full);
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
