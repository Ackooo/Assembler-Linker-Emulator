#pragma once

#include <string>
#include <regex>
#include <list>
#include <iomanip> //setw
#include <iostream>
#include <fstream>
#include <sstream>

#include "Symbol_table.h"
#include "Relocation_table.h"

using namespace std;

// asembler -o main.o main.s						
// asembler -o math.o math.s
// asembler -o ivt.o ivt.s			
// asembler -o isr_reset.o isr_reset.s
// asembler -o isr_terminal.o isr_terminal.s
// asembler -o isr_timer.o isr_timer.s
// asembler -o isr_user0.o isr_user0.s				
// linker -hex -o program.hex ivt.o math.o main.o isr_reset.o isr_terminal.o isr_timer.o isr_user0.o
//emulator program.hex

class Assembler {
private:
	string input_file_name;
	string output_file_name;
	Relocation_table relocation_table;
	Symbol_table symbol_table;
	int location_counter;
	int current_section;
	string output_code = "";

	ifstream input_file;
	ofstream output_file;

	int line_number;
	bool run;
	int instruction_size = 0;


public:
	Assembler(string in_file, string out_file);
	~Assembler();
	void assemble();
	bool backPatch();
	vector<string> format_line(string line);
	void print();
	string register_value(string reg);
	string format_decimal(int dec);
	void format_instruction_PC(string name, bool pc = false);
	string format_jump(string name);
};