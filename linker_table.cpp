#include "Linker_table.h"

	void Linker_table::add(string name, int section, int offset, int num, int size, char type, int file_num) {
		Linker_table_entry s(name, section, offset, num, size, type, file_num);
		this->linker_symbol_list.push_back(s);
	}

	list<Linker_table_entry> Linker_table::get_symbol_list() {
		return linker_symbol_list;
	}

	bool Linker_table::check_if_exists(string name) {
		for (Linker_table_entry& s : linker_symbol_list) {
			if (s.get_name() == name) {
				return true;
			}
		}
		return false;
	}
	bool Linker_table::check_if_global(string name) {
		for (Linker_table_entry& s : linker_symbol_list) {
			if (s.get_name() == name && s.get_type() == 'g') {
				return true;
			}
		}
		return false;
	}
	void Linker_table::set_size(string name, int size) {
		for (Linker_table_entry& s : linker_symbol_list) {
			if (s.get_name() == name) {
				s.set_size(size);
			}
		}
	}
	void Linker_table::set_offset(string name, int offset) {
		for (Linker_table_entry& s : linker_symbol_list) {
			if (s.get_name() == name) {
				s.set_offset(offset);
			}
		}
	}
	void Linker_table::set_offset_file(string name, int offset, int file_num) {
		for (Linker_table_entry& s : linker_symbol_list) {
			if (s.get_name() == name && s.get_file_num() == file_num) {
				s.set_offset(offset);
			}
		}
	}