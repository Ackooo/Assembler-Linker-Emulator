#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

using namespace std;

const regex linker_symbol_table_regex("^[\\s]*(linker - tabela simbola)[\\s]*$");

class Emulator {
private:
	ifstream input_file;
	string input_file_name;
	ofstream output_file;

	unsigned short memory_cap;
	string* memory;
	string curr_address;

	bool run = true;

	unsigned short registers[9];	//psw-r8

	//ivt - 0x0000 - 8 ulaza x 2B
	//	0	-	pokretanje
	//	1	-	nekorektna instrukcija
	//	2	-	tajmer
	//	3	-	terminal
	//	ostalo - slobodno
	//
	//memorijski mapirani registri - 0xff00 - 256B
	//65280
	//
	//r7-pc
	//r6-sp - zauzeta lok na vrhu steka - stek raste ka nizim
	//psw
	//000 NCOZ
	//neg-carry-overflow-zero

	int print_num = 0;

public:
	Emulator(string in_file);
	~Emulator();
	void emulate();
	string get_byte(int pos);
	void set_byte(int pos, string byte);
	void set_2_bytes_le(int pos, string byte);
	string get_2_bytes(int pos);
	string get_2_bytes_little_endian(int pos);
	int convert_hexa_to_dec(string hexa);
	string convert_dec_to_hexa(int dec);
	void push(string reg);
	void push(int reg);
	void pop(string reg);
	void pop(int reg);
	int register_value(string reg);
	int get_I(); 
	int get_N(); 
	int get_C(); 
	int get_O(); 
	int get_Z(); 
	void set_I(int num); 
	void set_N(int num); 
	void set_C(int num); 
	void set_O(int num); 
	void set_Z(int num); 
	int get_high_4_bits(string str); 
	int get_low_4_bits(string str); 
	void print_curr(string instruction);
	void reset();
	void err(int intr);
	void exit_print();
};