#pragma once

#include <string>
#include <ostream>
#include <iomanip>

using namespace std;

class Linker_table_entry {
private:
	string name;
	int section;
	int offset;
	int num;
	int size;
	char type;
	int file_num;
public:
	Linker_table_entry(string name, int section, int offset, int num, int size, char type, int file_num) {
		this->name = name;
		this->section = section;
		this->offset = offset;
		this->num = num;
		this->size = size;
		this->type = type;
		this->file_num = file_num;
	}

	string get_name() {
		return name;
	}
	char get_type() {
		return type;
	}
	int get_file_num() {
		return file_num;
	}
	int get_size() {
		return size;
	}
	int get_section() {
		return section;
	}
	int get_offset() {
		return offset;
	}
	int get_num() {
		return num;
	}

	void set_size(int size) {
		this->size = size;
	}
	void set_offset(int offset) {
		this->offset = offset;
	}

	friend ostream& operator<<(ostream& op, const Linker_table_entry& sym) {
		op << " " << setw(12) << sym.name;
		op << " " << setw(10) << sym.section;
		op << " " << setw(10) << sym.offset;
		op << " " << setw(10) << sym.num;
		op << " " << setw(10) << sym.size;
		op << " " << setw(10) << sym.type;
		op << " " << setw(10) << sym.file_num << endl;
		return op;
	}
};