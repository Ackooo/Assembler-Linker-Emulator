#pragma once

#include <list>
#include <iomanip>
#include "Relocation_table_entry.h"

using namespace std;

class Relocation_table {
private:
	list<Relocation_table_entry> relocation_list;
public:
	Relocation_table() {

	}
	void add(int section, int offset, int TSref, char type, bool isPC=false) {
		Relocation_table_entry r(section, offset, TSref, type, isPC);
		this->relocation_list.push_back(r);
	}

	list<Relocation_table_entry> get_relocation_list() {
		return relocation_list;
	}

	friend ostream& operator<<(ostream& op, const Relocation_table& sym) {
		op << "relokaciona tabela" << endl;
		op << " " << setw(10) << "sekcija";
		op << " " << setw(10) << "ofset";
		op << " " << setw(10) << "pc?";
		op << " " << setw(10) << "referenca na TS";
		op << " " << setw(10) << "tip"<<endl;
		for (auto const& sim : sym.relocation_list) {
			op << sim;
		}
		return op;
	}
};
