#pragma once

#include "linker_table.h"
#include "Linker_relocation_table.h"
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Linker {
private:
	vector<string> input_name_list;
	ifstream* input_file_list;
	ofstream output_file;
	int file_number;
	Linker_table linker_symbol_table;
	Linker_relocation_table linker_relocation_table;
	map<string, string> sections;
	map<string, int> section_sizes;
	map<string, int> section_counts;
	map<int, string> section_priority;
	int section_priority_curr;
	string output_code = "";

public:
	Linker(vector<string> input_name_list, string output_file_name);
	~Linker();
	void link();
	string format_string(string input);
	string format_line(string inp);
	string format_adress(int dec);
	void print();
	void err();
};