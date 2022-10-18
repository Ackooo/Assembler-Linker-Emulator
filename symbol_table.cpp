#include "symbol_table.h"


using namespace std;

void Symbol_table::add(string name, int section, int offset, char type) {
	Symbol_table_entry s(name, section, offset, type);
	this->symbol_list.push_back(s);
}
list<Symbol_table_entry> Symbol_table::get_symbol_list() {
	return symbol_list;
}
bool Symbol_table::check_if_exists(string name) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			return true;
		}
	}
	return false;
}
bool Symbol_table::check_global(string name) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			return s.get_global();
		}
	}
	cout << "***GRESKA*** simbol " <<name<<" nije u tabeli";
	exit(-1);
	return false;
}
void Symbol_table::set_global(string name) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			s.set_global(true);
		}
	}
}
void Symbol_table::set_size_section(int section, int size) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_section() == section && s.get_section() == s.get_number() && s.get_type() == 's') {
			s.set_size(size);
		}
	}
}
void Symbol_table::set_size_section(string section, int size) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == section) {
			s.set_size(size);
		}
	}
}
int Symbol_table::get_section(string name) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			return s.get_section();
		}
	}
	return 0;
}
int Symbol_table::get_offset(string name) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			return s.get_offset();
		}
	}
	cout << "***GRESKA*** nedefinisan simbol - offset - " <<name<< endl;
	exit(-1);
	return 0;
}

bool Symbol_table::check_defined(string name) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			if (s.get_type() == 'u') {
				return false;
			}
			else {
				true;
			}
		}
	}
	cout << "***GRESKA*** nije u tabeli simbol "<<name<<endl;
	exit(-1);
	return false;
}
char Symbol_table::get_type(string name) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			return s.get_type();
		}
	}
}
void Symbol_table::set_type(string name, char type) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			s.set_type(type);
		}
	}
}
void Symbol_table::add_patch(string name, int section, int location, bool pc) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			s.add_patch(name, section, location, pc, s.get_type());
		}
	}
}
void Symbol_table::set_section(string name, int section) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			s.set_section(section);
		}
	}
}
void Symbol_table::set_offset(string name, int offset) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			s.set_offset(offset);
		}
	}
}
int Symbol_table::get_number(string name) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			return s.get_number();
		}
	}
}
void Symbol_table::set_patch_type(string name, char type) {
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_name() == name) {
			s.set_patch_type(type);
		}
	}
}
int Symbol_table::get_id() {
	int m = 0;
	for (Symbol_table_entry& s : symbol_list) {
		if (s.get_number() >m) {
			m=s.get_number();
		}
	}
	return m;
}

