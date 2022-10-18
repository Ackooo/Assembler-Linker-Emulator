#pragma once

#include <string>

using namespace std;

class Linker_relocation_table_entry {
private:
	int section;
	int offset;
	string isPC;
	int TSref;
	char type;
	int file_num;

public:
	Linker_relocation_table_entry(int section, int offset, string isPC, int TSref, char type, int file_num) {
		this->section = section;
		this->offset = offset;
		this->isPC = isPC;
		this->TSref = TSref;
		this->type = type;
		this->file_num = file_num;
	}

	friend ostream& operator<<(ostream& op, const Linker_relocation_table_entry& rel) {
		op << " " << setw(10) << rel.section;
		op << " " << setw(10) << rel.offset;
		op << " " << setw(10) << rel.isPC;
		op << " " << setw(10) << rel.TSref;
		op << " " << setw(10) << rel.type;
		op << " " << setw(10) << rel.file_num << endl;
		return op;
	}
	int get_section() {
		return section;
	}
	int get_offset() {
		return offset;
	}
	bool get_isPC() {
		if (isPC == "R_386_PC32") { return true; }	
		else { return false; }
	}
	int get_refTS() {
		return TSref;
	}
	int get_file_num() {
		return file_num;
	}
	char get_type() {
		return type;
	}
};
