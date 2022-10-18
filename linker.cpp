#include "linker.h"
#include <regex>

#include <iomanip>
//#include <vector>

using namespace std;

const regex symbol_table_regex("^[\\s]*(tabela simbola)[\\s]*$");
const regex relocation_table_regex("^[\\s]*(relokaciona tabela)[\\s]*$");

const regex ob_format_regex("^.*\.(o)[\\s]*$");
const regex hex_format_regex("^.*\.(hex)[\\s]*$");

Linker::Linker(vector<string> input_name_list, string output_file_name) {
	this->input_name_list = input_name_list;
	this->file_number = input_name_list.size();

	this->input_file_list = new ifstream[input_name_list.size()];
	if (!regex_match(output_file_name, hex_format_regex)) {
		cout << "***GRESKA*** izlazni fajl nije u dobrom formatu" << endl;
	}

	this->output_file.open(output_file_name);
	if (!output_file.is_open()) {
		cout << "***GRESKA*** nije otvoren izlazni fajl" << endl;
		exit(-1);
	}

	this->section_priority_curr = 0;

	for (int i = 0; i < input_name_list.size(); i++) {
		if (!regex_match(input_name_list[i], ob_format_regex)) {
			for (int j = 0; j < i; j++) {
				input_file_list[j].close();
			}
			cout << "***GRESKA*** ulazni fajl nije u dobrom formatu - " << input_name_list[i] << endl;
			exit(-1);
		}
		input_file_list[i].open(input_name_list[i]);
		if (!input_file_list[i].is_open()) {
			for (int j = 0; j < i; j++) {
				input_file_list[j].close();
			}
			cout << "***GRESKA*** nije otvoren ulazni fajl " << input_name_list[i] << endl;
			exit(-1);
		}
	}
}
Linker::~Linker() {
	for (int i = 0; i < input_name_list.size(); i++) {
		input_file_list[i].close();
	}
	output_file.close();
}
void Linker::link() {
	for (int i = 0; i < input_name_list.size(); i++) {
		string line = "";
		int curr = 0;

		while (getline(input_file_list[i], line)) {
			//empty
			if (line.empty()) {
				continue;
				cout << "empty" << endl;
			}

			if (regex_match(line, symbol_table_regex)) {
				curr = 1;
				getline(input_file_list[i], line);//header
				continue;
			}

			if (regex_match(line, relocation_table_regex)) {
				curr = 2;
				getline(input_file_list[i], line);
				continue;
			}

			if (curr == 0) {
				//sekcije i kodovi
				string section = line;
				string code;
				getline(input_file_list[i], code);

				map<string, string>::iterator it = sections.find(line);
				if (it == sections.end()) {
					//not found
					sections.insert({ section, code });
				}
				else {
					//found
					it->second += code;
				}
			}
			else if (curr == 1) {
				// tabela simbola
				vector<string> lista;
				stringstream sline(line);
				do {
					string el;
					sline >> el;
					if (!el.size())continue;
					lista.push_back(el);
				} while (sline);
				char type = lista[6][0];

				if (lista[0] == "UND")continue;
				if (type == 'l') {
					//continue; 
				}
				else if (type == 'g') {
					bool exists = linker_symbol_table.check_if_global(lista[0]);
					if (exists) {
						cout << "***GRESKA*** simbol " << lista[0] << " izvezen vise puta" << endl;
						err();
					}
				}
				else if (type == 'e') {

				}
				else if (type == 's') {
					map<string, int>::iterator it_s = section_sizes.find(lista[0]);
					if (it_s == section_sizes.end()) {
						//not found					
						section_sizes.insert({ lista[0], stoi(lista[5]) });
						section_counts.insert({ lista[0], 1 });
						linker_symbol_table.add(lista[0], stoi(lista[1]), 0, stoi(lista[4]), stoi(lista[5]), type, i);
						section_priority.insert({ section_priority_curr++, lista[0] });
					}
					else {
						//found
						int ofs = it_s->second;
						linker_symbol_table.add(lista[0], stoi(lista[1]), ofs, stoi(lista[4]), stoi(lista[5]), type, i);
						it_s->second += stoi(lista[5]);
						map<string, int>::iterator it_c = section_counts.find(lista[0]);
						it_c->second++;
					}
				}
				if (type != 's') {
					linker_symbol_table.add(lista[0], stoi(lista[1]), stoi(lista[2]), stoi(lista[4]), stoi(lista[5]), type, i);
				}
			}
			else {
				//reloaciona tabela
				vector<string> lista;

				stringstream sline(line);
				do {
					string el;
					sline >> el;
					if (!el.size())continue;
					lista.push_back(el);
				} while (sline);
				char type = lista[4][0];
				linker_relocation_table.add(stoi(lista[0]), stoi(lista[1]), lista[2], stoi(lista[3]), type, i);
			}
		}//getline
	}//fajl

	//check undefined extern
	for (Linker_table_entry& s : linker_symbol_table.get_symbol_list()) {
		if (s.get_type() == 'e') {
			bool is_undefined = linker_symbol_table.check_if_global(s.get_name());
			if (!is_undefined) {
				cout << "***GRESKA*** simbol " << s.get_name() << " nije izvezen" << endl;
				err();
			}
		}
	}

	int linker_lc = 0;

	//set offset s
	map<int, string>::iterator pom;
	for (pom = section_priority.begin(); pom != section_priority.end(); pom++) {
		map<string, string>::iterator it_s = sections.find(pom->second);
		output_code += it_s->second;
		int curr = linker_lc;
		map<string, int>::iterator it = section_sizes.find(pom->second);
		for (Linker_table_entry& s : linker_symbol_table.get_symbol_list()) {
			if (s.get_name() == it->first) {
				int file_num = s.get_file_num();
				linker_symbol_table.set_offset_file(s.get_name(), curr, file_num);
				curr += s.get_size();
			}
		}
		linker_lc += it->second;
	}

	linker_lc = 0;

	//set offset l/g
	for (Linker_table_entry& s : linker_symbol_table.get_symbol_list()) {
		if (s.get_type() == 'l' || s.get_type() == 'g') {
			int section_ofs = -1;
			for (Linker_table_entry& ss : linker_symbol_table.get_symbol_list()) {
				if (s.get_section() == ss.get_num() && ss.get_file_num() == s.get_file_num()) {
					section_ofs = ss.get_offset();
				}
			}
			if (section_ofs == -1) {
				cout << "***GRESKA***  nije odredjen ofset simbolu " << s.get_name() << endl;
				err();
			}
			linker_symbol_table.set_offset(s.get_name(), section_ofs + s.get_offset());
		}		
	}

	//relocation adress fix
	for (Linker_relocation_table_entry& l : linker_relocation_table.get_relocation_list()) {
		int section = l.get_section();
		int offset = l.get_offset();
		bool isPC = l.get_isPC();
		int refTS = l.get_refTS();
		char type = l.get_type();
		int file_num = l.get_file_num();

		int position = -1;
		int location = -1;

		if (type == 'l' || type == 'g') {
			for (Linker_table_entry& ss : linker_symbol_table.get_symbol_list()) {
				if (section == ss.get_num() && ss.get_file_num() == file_num) {
					position = ss.get_offset();
				}
				if (ss.get_file_num() == file_num && refTS == ss.get_num()) {
					location = ss.get_offset();
					//ofset ukupan a ne lokalan u sekciji
				}
			}
		}
		else if (type == 'e') {
			string name = "";

			for (Linker_table_entry& ss : linker_symbol_table.get_symbol_list()) {
				if (section == ss.get_num() && ss.get_file_num() == file_num) {
					position = ss.get_offset();
				}

				if (ss.get_file_num() == file_num && refTS == ss.get_num()) {
					name = ss.get_name();
				}
			}
			if (name == "") {
				cout << "***GRESKA*** nije pronadjen simbol - ts ref: " << refTS << " fajl: "<<file_num<<" u tabeli simbola prilikom obilaska relokacione tabele" <<endl;
				err();
			}
			for (Linker_table_entry& ss : linker_symbol_table.get_symbol_list()) {
				if (ss.get_name() == name && ss.get_type() == 'g') {
					location = ss.get_offset();
				}
			}
		}
		else {
			cout << "***GRESKA***  nije odredjen tip simbolu u relokacionoj tabeli - ts ref: "<< refTS << " fajl: " << file_num<<endl;
			err();
		}
		if (position == -1) {
			cout << "***GRESKA***  nije odredjena pozicija simbolu u relokacionoj tabeli - ts ref: " << refTS << " fajl: " << file_num << endl;
			err();

		}
		if (location == -1) {
			cout << "***GRESKA***  nije odredjena lokacija simbolu u relokacionoj tabeli - ts ref: " << refTS << " fajl: " << file_num << endl;
			err();
		}
		position += offset;

		string location_string = "";
		if (isPC) {
			cout << location << endl << position << endl << endl;
			int t = location - position;
			location_string = format_adress(t);
		}
		else {
			location_string = format_adress(location);
		}
		if (location_string == "") {
			cout << "***GRESKA***  nije odredjena lokacija simbolu u relokacionoj tabeli - ts ref: " << refTS << " fajl: " << file_num << endl;
			err();
		};

		//little endian
		output_code[position * 2] = location_string[2];
		output_code[position * 2 + 1] = location_string[3];
		output_code[position * 2 + 2] = location_string[0];
		output_code[position * 2 + 3] = location_string[1];
	}
	
	output_code = format_string(output_code);
	output_code = format_line(output_code);
	print();
	cout << "link complete successfully" << endl;

}//link

string Linker::format_string(string input) {
	int a = input.length() / 2 - 1;
	string output;
	for (int i = 0; i <= a; i++) {
		output.push_back(input[i * 2 + 0]);
		output.push_back(input[i * 2 + 1]);
		output.push_back(' ');
	}
	return output;
}

string Linker::format_line(string inp) {
	int a = inp.length() / 21;
	string output;
	int adr = 0;
	string input = inp;
	for (int i = 0; i <= a; i++) {
		string adrr = format_adress(adr);
		adr += 8;
		output += adrr;
		output += ": ";
		//
		string red = input.substr(0, 24);
		input.erase(0, 24);
		output += red;
		//
		output += '\n';
	}
	return output;
}

string Linker::format_adress(int dec) {
	if (dec >= 0) {
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(4) << std::hex << dec;
		std::string result(stream.str());
		return result;
	}
	else {
		std::stringstream stream;
		stream << std::setfill('F') << std::setw(4) << std::hex << dec;
		std::string result(stream.str());
		result.erase(0, 4);
		return result;
	}
}

void Linker::print() {
	output_file << output_code << '\n' << endl;
	output_file << linker_symbol_table << '\n' << endl;
	output_file << linker_relocation_table << '\n' << endl;
	map<string, int>::iterator it;
	for (it = section_sizes.begin(); it != section_sizes.end(); it++) {
		output_file << it->first << endl;
		output_file << it->second << endl;
		output_file << endl;
	}
	for (it = section_counts.begin(); it != section_counts.end(); it++) {
		output_file << it->first << endl;
		output_file << it->second << endl;
		output_file << endl;
	}
	map<string, string>::iterator it2;
	for (it2 = sections.begin(); it2 != sections.end(); it2++) {
		output_file << it2->first << endl;
		output_file << it2->second << endl;
		output_file << endl;
	}
}
void Linker::err() {
	for (int i = 0; i < input_name_list.size(); i++) {
		input_file_list[i].close();
	}
	output_file.close();
	exit(-1);
}