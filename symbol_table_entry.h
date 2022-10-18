#pragma once

#include <string>
#include "patch.h"
#include <iomanip>
#include <iostream>
#include <list>


using namespace std;

static int symbol_table_entry_id = 0;

class Symbol_table_entry {
private:
	string name;
	int section;
	int offset;
	bool isGlobal;
	int num;
	int size;
	char type;
	list<Patch> patch_list;

public:
	Symbol_table_entry(string name, int section, int offset, char type) {
		this->name = name;
		this->section = section;
		this->offset = offset;
		this->num = symbol_table_entry_id++;
		this->isGlobal = false;//lokalni automatski
		this->size = -1;
		this->type = type;
	}

	friend ostream& operator<<(ostream& op, const Symbol_table_entry& sym) {
		op << " " << setw(12) << sym.name;
		op << " " << setw(10) << sym.section;
		op << " " << setw(10) << sym.offset;
		if (sym.isGlobal) {
			op << " " << setw(10) << "globalna";
		}
		else {
			op << " " << setw(10) << "lokalna";
		}
		op << " " << setw(10) << sym.num;
		op << " " << setw(10) << sym.size;
		op << " " << setw(10) << sym.type << endl;
		return op;
	}

	void add_patch(string name, int section, int location, bool pc, char type) {
		Patch a(name, section, location, pc, type);
		patch_list.push_back(a);
	}
	list<Patch> get_patch_list() {
		return patch_list;
	}

	void set_patch_type(char type) {
		for (list<Patch>::iterator it = patch_list.begin(); it != patch_list.end(); it++) {
			it->set_type(type);
		}
	}

	void set_global(bool isglobal) {
		this->isGlobal = isglobal;
	}
	bool get_global() {
		return isGlobal;
	}
	string get_name() {
		return name;
	}
	int get_size() {
		return size;
	}
	void set_size(int size) {
		this->size = size;
	}
	int get_section() {
		return section;
	}
	int get_number() {
		return num;
	}
	int get_offset() {
		return offset;
	}
	void set_offset(int offset) {
		this->offset = offset;
	}
	void set_type(char type) {
		this->type = type;
	}
	char get_type() {
		return type;
	}
	void set_section(int section) {
		this->section = section;
	}
};