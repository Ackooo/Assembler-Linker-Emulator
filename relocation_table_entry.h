#pragma once

#include <string>
#include <iomanip>

using namespace std;

class Relocation_table_entry {
private:
	int section;//adresa
	int offset;//adresa
	bool isPC;//pc=true
	int STref;
	char type;

public:
	Relocation_table_entry(int section, int offset, int TSref, char type, bool isPC = false) {
		this->section = section;
		this->offset = offset;
		this->STref = TSref;
		this->isPC = isPC;
		this->type = type;
	}
	
	friend ostream& operator<<(ostream& op, const Relocation_table_entry& rel) {
		op << " " << setw(10) << rel.section;
		op << " " << setw(10) << rel.offset;
		if (rel.isPC) {
			op << " " << setw(10) << "R_386_PC32";
		}
		else {
			op << " " << setw(10) << "R_386_32";
		}
		op << " " << setw(10) << rel.STref;
		op << " " << setw(10) << rel.type << endl;
		return op;
	}
};