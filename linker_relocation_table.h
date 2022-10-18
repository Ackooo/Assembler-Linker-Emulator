#pragma once

#include "Linker_relocation_table_entry.h"
#include <list>
using namespace std;

class Linker_relocation_table {
private:
	list<Linker_relocation_table_entry> linker_relocation_list;
public:
	Linker_relocation_table() {

	}
	void add(int section, int offset, string isPC, int TSref, char type, int file_num) {
		Linker_relocation_table_entry s(section, offset, isPC, TSref, type, file_num);
		this->linker_relocation_list.push_back(s);
	}
	list<Linker_relocation_table_entry> get_relocation_list() {
		return linker_relocation_list;
	}
	friend ostream& operator<<(ostream& op, const Linker_relocation_table& sym) {
		op << "linker-relokaciona tabela" << endl;
		op << " " << setw(10) << "sekcija";
		op << " " << setw(10) << "ofset";
		op << " " << setw(10) << "pc?";
		op << " " << setw(10) << "referenca na TS";
		op << " " << setw(10) << "tip";
		op << " " << setw(10) << "fajl" << endl;
		for (auto const& sim : sym.linker_relocation_list) {
			op << sim;
		}
		return op;
	}
};