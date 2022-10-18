#pragma once

#include <string>

using namespace std;

class Patch
{
private:
	string name;
	int section;
	int location;
	bool pc;
	char type;
public:
	Patch(string name, int section, int location, bool isPC, char type) {
		this->name = name;
		this->section = section;
		this->location = location;
		this->pc = isPC;
		this->type = type;
	}

	string get_name() {
		return name;
	}
	int get_section() {
		return section;
	}
	int get_location() {
		return location;
	}
	bool get_isPC() {
		return pc;
	}
	char get_type() {
		return type;
	}
	void set_type(char type) {
		this->type = type;
	}
};
