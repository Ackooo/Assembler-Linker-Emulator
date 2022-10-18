#pragma once

#include "Linker_table_entry.h"
#include <list>
using namespace std;

class Linker_table {
private:
	list<Linker_table_entry> linker_symbol_list;
public:
	Linker_table() {

	}
	friend ostream& operator<<(ostream& op, const Linker_table& sym) {
		op << "linker - tabela simbola" << endl;
		op << " " << setw(12) << "name/label";
		op << " " << setw(10) << "sekcija";
		op << " " << setw(10) << "ofset";
		op << " " << setw(10) << "redni broj";
		op << " " << setw(10) << "velicina";
		op << " " << setw(10) << "tip";
		op << " " << setw(10) << "fajl" << endl;
		for (auto const& sim : sym.linker_symbol_list) {
			op << sim;
		}
		return op;
	}
	
	void add(string name, int section, int offset, int num, int size, char type, int file_num); 
	list<Linker_table_entry> get_symbol_list();
	bool check_if_exists(string name);
	bool check_if_global(string name);
	void set_size(string name, int size);
	void set_offset(string name, int offset);
	void set_offset_file(string name, int offset, int file_num); 
};
