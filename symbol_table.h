#pragma once

#include "symbol_table_entry.h"
#include <list>
#include <iostream>
#include <iomanip>

using namespace std;

class Symbol_table {
private:
	list<Symbol_table_entry> symbol_list;
public:
	Symbol_table() {

	}
	friend ostream& operator<<(ostream& op, const Symbol_table& sym) {
		op << "tabela simbola" << endl;
		op << " " << setw(12) << "labela";
		op << " " << setw(10) << "sekcija";
		op << " " << setw(10) << "ofset";
		op << " " << setw(10) << "globalna?";
		op << " " << setw(10) << "redni broj";
		op << " " << setw(10) << "velicina";
		op << " " << setw(10) << "tip" << endl;

		for (auto const& sim : sym.symbol_list) {
			op << sim;
		}
		return op;
	}

	void add(string name, int section, int offset, char type = 'u');
	list<Symbol_table_entry> get_symbol_list();
	bool check_if_exists(string name);
	bool check_global(string name);
	void set_global(string name);
	void set_size_section(int section, int size);
	void set_size_section(string section, int size);
	int get_section(string name);
	int get_offset(string name);
	bool check_defined(string name);
	char get_type(string name);
	void set_type(string name, char type);
	void add_patch(string name, int section, int location, bool pc = false);
	void set_section(string name, int section);
	void set_offset(string name, int offset);
	int get_number(string name);
	void set_patch_type(string name, char type);
	int get_id();
};