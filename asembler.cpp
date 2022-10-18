#include "asembler.h"

const regex label_regex("^[\\s]*[A-Za-z_]+:^[\\s]*$");//labela sama
const regex label_line_regex("^[\\s]*([A-Za-z_0-9]+:)(.*)");//labela postoji
const regex comment_regex("^[\\s]*(#)(.*)");
const regex section_regex("^[\\s]*(.section)[\\s]+([a-zA-Z_0-9]+)[\\s]*$");
const regex word_regex("^[\\s]*(.word)[\\s]+(([a-zA-Z_0-9]+)([,]?)([\\s]?))+$");
const regex skip_regex("^[\\s]*(.skip)[\\s]+([a-zA-Z0-9]+)[\\s]*$");
const regex global_regex("^[\\s]*(.global)[\\s]+(([a-zA-Z_0-9]+)([,]?)([\\s]?))+[\\s]*$");
const regex extern_regex("^[\\s]*(.extern)[\\s]+(([a-zA-Z_0-9]+)([,]?)([\\s]?))+[\\s]*$");

const regex decimal_regex("^([0-9]+)$");
const regex hexadecimal_regex("^(0)(x|X)[a-fA-F]+$");

const regex end_regex("^[\\s]*(.end)[\\s]*$");

const regex halt_regex("^[\\s]*(halt)[\\s]*$");
const regex int_regex("^[\\s]*(int)[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");
const regex iret_regex("^[\\s]*(iret)[\\s]*$");

const regex call_literal_regex("^[\\s]*(call)[\\s]+([xa-fA-F0-9]+)[\\s]*$");
const regex call_symbol_regex("^[\\s]*(call)[\\s]+([a-zA-Z0-9_]+)[\\s]*$");
const regex call_symbol_PC_regex("^[\\s]*(call)[\\s]+(%[a-zA-Z0-9_]+)[\\s]*$");
const regex call_literal_MEM_regex("^[\\s]*(call)[\\s]+(\\*[xa-fA-F0-9]+)[\\s]*$");
const regex call_symbol_MEM_regex("^[\\s]*(call)[\\s]+(\\*[a-zA-Z0-9_]+)[\\s]*$");
const regex call_reg_regex("^[\\s]*(call)[\\s]+\\*((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");
const regex call_reg_MEM_regex("^[\\s]*(call)[\\s]+\\*\\[((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*\\][\\s]*$");
const regex call_regIND_literal_regex("^[\\s]*(call)[\\s]+\\*\\[((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]+\\+[\\s]+[xa-fA-F0-9]+[\\s]*\\][\\s]*$");
const regex call_regIND_symbol_regex("^[\\s]*(call)[\\s]+\\*\\[((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]+\\+[\\s]+[a-zA-Z_0-9]+[\\s]*\\][\\s]*$");

const regex ret_regex("^[\\s]*(ret)[\\s]*$");

//jmp, jeq ,jne,jgt
const regex jump_literal_regex("^[\\s]*((jmp)|(jeq)|(jne)|(jgt))[\\s]+([xa-fA-F0-9]+)[\\s]*$");
const regex jump_symbol_regex("^[\\s]*((jmp)|(jeq)|(jne)|(jgt))[\\s]+([a-zA-Z0-9_]+)[\\s]*$");
const regex jump_symbol_PC_regex("^[\\s]*((jmp)|(jeq)|(jne)|(jgt))[\\s]+(%[a-zA-Z0-9_]+)[\\s]*$");
const regex jump_literal_MEM_regex("^[\\s]*((jmp)|(jeq)|(jne)|(jgt))[\\s]+(\\*[xa-fA-F0-9]+)[\\s]*$");
const regex jump_symbol_MEM_regex("^[\\s]*((jmp)|(jeq)|(jne)|(jgt))[\\s]+(\\*[a-zA-Z0-9_]+)[\\s]*$");
const regex jump_reg_regex("^[\\s]*((jmp)|(jeq)|(jne)|(jgt))[\\s]+\\*((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");
const regex jump_reg_MEM_regex("^[\\s]*((jmp)|(jeq)|(jne)|(jgt))[\\s]+\\*\\[((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*\\][\\s]*$");
const regex jump_regIND_literal_regex("^[\\s]*((jmp)|(jeq)|(jne)|(jgt))[\\s]+\\*\\[((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]+\\+[\\s]+[xa-fA-F0-9]+[\\s]*\\][\\s]*$");
const regex jump_regIND_symbol_regex("^[\\s]*((jmp)|(jeq)|(jne)|(jgt))[\\s]+\\*\\[((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]+\\+[\\s]+[a-zA-Z_0-9]+[\\s]*\\][\\s]*$");

const regex push_regex("^[\\s]*(push)[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");
const regex pop_regex("^[\\s]*(pop)[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");
const regex xchg_regex("^[\\s]*(xchg)[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");
const regex aritmetic_regex("^[\\s]*((add)|(sub)|(mul)|(div))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");//add, sub, mul, div, 
const regex cmp_regex("^[\\s]*(cmp)[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");
const regex not_regex("^[\\s]*(not)[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");
const regex logic_regex("^[\\s]*((and)|(or)|(xor))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");//and, or, xor
const regex test_regex("^[\\s]*(test)[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");
const regex shift_regex("^[\\s]*((shl)|(shr))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");

const regex ldrstr_literal_regex("^[\\s]*((ldr)|(str))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)(\\$[xa-fA-F0-9]+)[\\s]*$");
const regex ldrstr_symbol_regex("^[\\s]*((ldr)|(str))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)(\\$[a-zA-Z0-9_]+)[\\s]*$");
const regex ldrstr_literal_MEM_regex("^[\\s]*((ldr)|(str))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)([xa-fA-F0-9]+)[\\s]*$");
const regex ldrstr_symbol_MEM_APS_regex("^[\\s]*((ldr)|(str))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)([a-zA-Z0-9_]+)[\\s]*$");
const regex ldrstr_symbol_PC_regex("^[\\s]*((ldr)|(str))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)(%[a-zA-Z0-9_]+)[\\s]*$");
const regex ldrstr_reg_regex("^[\\s]*((ldr)|(str))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*$");
const regex ldrstr_reg_MEM_regex("^[\\s]*((ldr)|(str))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)\\[[\\s]*((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*\\][\\s]*$");
const regex ldrstr_regIND_literal_regex("[\\s]*((ldr)|(str))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)\\[((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]+\\+[\\s]+[xa-fA-F0-9]+[\\s]*\\][\\s]*$");
const regex ldrstr_regIND_symbol_regex("[\\s]*((ldr)|(str))[\\s]+((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]*([,]{1})([\\s]*)\\[((r0)|(r1)|(r2)|(r3)|(r4)|(r5)|(r6)|(r7)|(psw)|(sp)|(pc))[\\s]+\\+[\\s]+[a-zA-Z_0-9]+[\\s]*\\][\\s]*$");

const regex o_format_regex("^.*\.(o)[\\s]*$");
const regex s_format_regex("^.*\.(s)[\\s]*$");

Assembler::Assembler(string in_file, string out_file) {
	this->input_file_name = in_file;
	this->output_file_name = out_file;
	this->location_counter = 0;
	this->line_number = 0;
	this->run = true;
	this->current_section = -1;

	if (!regex_match(in_file, s_format_regex)) {
		cout << "***GRESKA*** ulazni fajl nije u dobrom formatu" << endl;
		exit(-1);
	}
	if (!regex_match(out_file, o_format_regex)) {
		cout << "***GRESKA*** izlazni fajl nije u dobrom formatu" << endl;
		exit(-1);
	}
	this->input_file.open(input_file_name); //ios::in
	this->output_file.open(output_file_name);//ios::binary|ios::out|ios::trunc // +".txt
	if (!input_file.is_open()) {
		cout << "***GRESKA*** nije otvoren ulazni fajl" << endl;
		exit(-1);
	}
	else if (!output_file.is_open()) {
		cout << "***GRESKA*** nije otvoren izlazni fajl" << endl;
		input_file.close();
		exit(-1);
	}
}
Assembler::~Assembler() {
	input_file.close();
	output_file.close();
}
void Assembler::assemble() {

	symbol_table.add("UND", 0, location_counter);
	current_section++;

	string line = "";
	while (getline(input_file, line) && run) {

		line_number++;

		//comm
		size_t imaKom = line.find("#");
		if (imaKom != string::npos) {
			line = line.substr(0, imaKom);
		}

		vector<string> code;
		code = format_line(line);

		//empty
		if (code.empty()) {
			continue;
		}

		//	*	*	*	LABEL	*	*	*
		if (regex_match(line, label_line_regex)) {
			if (current_section <= 0) {
				cout << "***GRESKA*** labela " << code[0] << " van sekcije" << endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			string labela = code.front();
			labela.erase(remove(labela.begin(), labela.end(), ':'), labela.end());

			bool labela_exists = symbol_table.check_if_exists(labela);

			if (labela_exists) {
				char a = symbol_table.get_type(labela);
				if (a == 'l') {
					cout << "***GRESKA*** simbol " << labela << "vec definisan" << endl;
					input_file.close();
					output_file.close();
					exit(-1);
				}
				else if (a == 'u') {
					bool gl = symbol_table.check_global(labela);

					symbol_table.set_section(labela, current_section);
					symbol_table.set_offset(labela, location_counter);
					if (gl) {
						symbol_table.set_type(labela, 'g');
						symbol_table.set_patch_type(labela, 'g');
					}
					else {
						symbol_table.set_type(labela, 'l');
						symbol_table.set_patch_type(labela, 'l');
					}
				}
				else {
					cout << "***GRESKA*** simbol " << labela << "vec definisana" << endl;
					input_file.close();
					output_file.close();
					exit(-1);
				}
			}
			else {
				symbol_table.add(labela, current_section, location_counter, 'l');
			}

			//izbaciti labelu iz line
			line.erase(line.find(labela), labela.length());
			line.erase(remove(line.begin(), line.end(), ':'), line.end());

			code = format_line(line);
			//labela sama
			if (code.empty()) {
				continue;
			}
		}
		//	*	*	*	ostalo	*	*	*

		//	*	*	*	SECTION	*	*	*
		if (regex_match(line, section_regex)) {
			string section = code.back();
			bool exists = symbol_table.check_if_exists(section);
			if (exists) {
				cout << "***GRESKA*** nije dozvoljeno ponovljanje sekcija - " << section << endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}

			if (current_section > 0) {
				symbol_table.set_size_section(current_section, location_counter);
			}

			current_section = symbol_table.get_id()+1;//symbol_table_entry_id;
			location_counter = 0;
			symbol_table.add(section, current_section, 0, 's');
			//
			output_code += "\n";
			output_code += section;
			output_code += "\n";
			continue;
		}
		//	*	*	*	WORD	*	*	*
		else if (regex_match(line, word_regex)) {
			if (current_section <= 0) {
				cout << "***GRESKA*** .word direktiva van sekcije" << endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}

			code.erase(code.begin());

			for (int i = 0; i < code.size(); i++) {
				if (regex_match(code[i], decimal_regex)) {
					string a = code[i];
					int b = stoi(a);
					output_code += format_decimal(b);
				}
				else if (regex_match(code[i], hexadecimal_regex)) {
					string a = code[i];
					int b = stoi(a, nullptr, 0);
					output_code += format_decimal(b);
				}
				else {
					bool exists = symbol_table.check_if_exists(code[i]);
					if (exists) {
						char a = symbol_table.get_type(code[i]);
						if (a == 'e') {
							output_code += "0000";
							symbol_table.add_patch(code[i], current_section, location_counter);
						}
						else if (a == 'g') {
							int a = symbol_table.get_offset(code[i]);
							output_code += format_decimal(a);
							symbol_table.add_patch(code[i], current_section, location_counter);
						}
						else if (a == 'l') {
							int a = symbol_table.get_offset(code[i]);
							output_code += format_decimal(a);
						}
						else if (a == 'u') {
							output_code += "0000";
							symbol_table.add_patch(code[i], current_section, location_counter);
						}
						else {
							cout << "***GRESKA*** " << code[i] << " vec definisan" << endl;
							input_file.close();
							output_file.close();
							exit(-1);
						}
					}
					else {
						symbol_table.add(code[i], current_section, location_counter);
						symbol_table.add_patch(code[i], current_section, location_counter);
						output_code += "0000";
					}
				}
				location_counter += 2;
			}
			continue;
		}
		//	*	*	*	SKIP	*	*	*
		else if (regex_match(line, skip_regex)) {
			if (current_section <= 0) {
				cout << "***GRESKA*** .skip direktiva van sekcije" << endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			if (regex_match(code.back(), decimal_regex)) {
				string a = code.back();
				int b = stoi(a);
				location_counter += b;
				for (int i = 0; i < b; i++) {
					output_code += "00";
				}
			}
			else if (regex_match(code.back(), hexadecimal_regex)) {
				string a = code.back();
				int b = stoi(a, nullptr, 0);
				location_counter += b;
				for (int i = 0; i < b; i++) {
					output_code += "00";
				}
			}
			else {
				cout << "***GRESKA*** na liniji " << line_number <<" - nije prepoznat literal" << endl;
				input_file.close();
				output_file.close();
				exit(-1);
			}
			continue;
		}
		//	*	*	*	GLOBAL	*	*	*
		else if (regex_match(line, global_regex)) {

			code.erase(code.begin());

			for (int i = 0; i < code.size(); i++) {
				bool exists = symbol_table.check_if_exists(code[i]);
				if (exists) {
					char a = symbol_table.get_type(code[i]);
					if (a == 'u') {
						symbol_table.set_global(code[i]);
					}
					else if (a == 'g') {
						//nebitan visak
					}
					else if (a == 'l') {
						symbol_table.set_global(code[i]);
						symbol_table.set_type(code[i], 'g');
						symbol_table.set_patch_type(code[i], 'g');
						//vrednosti jos neke?
					}
					else {
						cout << "***GRESKA*** simbol " << code[i] << " vec definisan - linija " << line_number << endl;
						input_file.close();
						output_file.close();
						exit(-1);
					}
				}
				else {
					symbol_table.add(code[i], current_section, 0);
					symbol_table.set_global(code[i]);
				}
			}
			continue;
		}
		//	*	*	*	EXTERN	*	*	*
		else if (regex_match(line, extern_regex)) {

			code.erase(code.begin());

			for (int i = 0; i < code.size(); i++) {
				bool exists = symbol_table.check_if_exists(code[i]);
				if (exists) {
					char a = symbol_table.get_type(code[i]);
					if (a == 'u') {
						symbol_table.set_type(code[i], 'e');
						symbol_table.set_global(code[i]);
						symbol_table.set_section(code[i], 0);
					}
					else if (a == 'e') {
						//viska 
					}
					else {
						cout << "***GRESKA*** simbol " << code[i] << " vec definisan - linija " << line_number << endl;
						input_file.close();
						output_file.close();
						exit(-1);
					}
				}
				else {
					symbol_table.add(code[i], 0, 0, 'e');//u undifined
					symbol_table.set_global(code[i]);
				}
			}
			continue;
		}
		//	*	*	*	END	*	*	*
		else if (regex_match(line, end_regex)) {
			run = false;
			symbol_table.set_size_section(current_section, location_counter);
			continue;
		}
		//	*	*	*	HALT	*	*	*
		else if (regex_match(line, halt_regex)) {
			instruction_size = 1;
			location_counter += 1;
			output_code += "00";
			continue;
		}
		//	*	*	*	INT	*	*	*
		else if (regex_match(line, int_regex)) {
			instruction_size = 2;
			location_counter += 2;
			output_code += "10";
			output_code += register_value(code.back());
			output_code += "f";
			continue;
		}
		//	*	*	*	IRET	*	*	*
		else if (regex_match(line, iret_regex)) {
			instruction_size = 1;
			location_counter += 1;
			output_code += "20";
			continue;
		}
		//	*	*	*	CALL LITERAL	*	*	*
		else if (regex_match(line, call_literal_regex)) {
			instruction_size = 5;
			location_counter += 5;
			output_code += "30f000";
			if (regex_match(code[1], decimal_regex)) {
				int a = stoi(code[1]);
				output_code += format_decimal(a);
			}
			else if (regex_match(code[1], hexadecimal_regex)) {
				int a = stoi(code[1], nullptr, 0);
				output_code += format_decimal(a);
			}
			else {
				cout << "***GRESKA*** na liniji " << line_number <<" - literal nije prepoznat"<< endl;
				input_file.close();
				output_file.close();
				exit(-1);
			}
			continue;
		}
		//	*	*	*	CALL SIMBOL	*	*	*
		else if (regex_match(line, call_symbol_regex)) {
			instruction_size = 5;
			location_counter += 3;
			output_code += "30f000";
			output_code += "0000";
			format_instruction_PC(code[1]);
			location_counter += 2;
			continue;
		}
		//	*	*	*	CALL %SIMBOL	*	*	*
		else if (regex_match(line, call_symbol_PC_regex)) {
			instruction_size = 5;
			location_counter += 3;
			output_code += "30f";
			output_code += "7";//pc
			output_code += "05";
			output_code += "0000";
			format_instruction_PC(code[1], true);
			location_counter += 2;
			continue;
		}
		//	*	*	*	CALL *LITERAL	*	*	*
		else if (regex_match(line, call_literal_MEM_regex)) {
			instruction_size = 5;
			location_counter += 5;
			output_code += "30f004";
			if (regex_match(code[1], decimal_regex)) {
				int a = stoi(code[1]);
				output_code += format_decimal(a);
			}
			else if (regex_match(code[1], hexadecimal_regex)) {
				int a = stoi(code[1], nullptr, 0);
				output_code += format_decimal(a);
			}
			else {
				cout << "***GRESKA*** na liniji " << line_number << " - nepoznat literal"<<endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			continue;
		}
		//	*	*	*	CALL *REG	*	*	*
		else if (regex_match(line, call_reg_regex)) {
			instruction_size = 3;
			location_counter += 3;
			output_code += "30f";
			output_code += register_value(code[1]);
			output_code += "01";
			continue;
		}
		//	*	*	*	CALL *SIMBOL	*	*	*
		else if (regex_match(line, call_symbol_MEM_regex)) {
			instruction_size = 5;
			location_counter += 3;
			output_code += "30f004";
			output_code += "0000";
			format_instruction_PC(code[1]);
			location_counter += 2;
			continue;
		}
		//	*	*	*	CALL *[REG]	*	*	*
		else if (regex_match(line, call_reg_MEM_regex)) {
			instruction_size = 3;
			location_counter += 3;
			output_code += "30f";
			output_code += register_value(code[1]);
			output_code += "02";
			continue;
		}
		//	*	*	*	CALL *[REG + L]	*	*	*
		else if (regex_match(line, call_regIND_literal_regex)) {
			instruction_size = 5;
			location_counter += 5;
			output_code += "30f";
			output_code += register_value(code[1]);
			output_code += "03";
			if (regex_match(code[2], decimal_regex)) {
				int a = stoi(code[2]);
				output_code += format_decimal(a);
			}
			else if (regex_match(code[2], hexadecimal_regex)) {
				int a = stoi(code[2], nullptr, 0);
				output_code += format_decimal(a);
			}
			else {
				cout << "***GRESKA*** na liniji " << line_number << " - nepoznat literal"<< endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			continue;
		}
		//	*	*	*	CALL *[REG + S]	*	*	*
		else if (regex_match(line, call_regIND_symbol_regex)) {
			instruction_size = 5;
			location_counter += 3;
			output_code += "30f";
			output_code += register_value(code[1]);
			output_code += "03";
			output_code += "0000";
			format_instruction_PC(code[2]);
			location_counter += 2;
			continue;
		}
		//	*	*	*	RET	*	*	*
		else if (regex_match(line, ret_regex)) {
			instruction_size = 1;
			location_counter += 1;
			output_code += "40";
			continue;
		}
		//	*	*	*	JUMP LITERAL	*	*	*
		else if (regex_match(line, jump_literal_regex)) {
			instruction_size = 5;
			location_counter += 5;
			output_code += "5";
			output_code += format_jump(code[0]);
			output_code += "f000";

			if (regex_match(code[1], decimal_regex)) {
				int a = stoi(code[1]);
				output_code += format_decimal(a);
			}
			else if (regex_match(code[1], hexadecimal_regex)) {
				int a = stoi(code[1], nullptr, 0);
				output_code += format_decimal(a);
			}
			else {
				cout << "***GRESKA*** na liniji " << line_number << " - nepoznat literal"<<endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			continue;
		}
		//	*	*	*	JUMP SIMBOL	*	*	*
		else if (regex_match(line, jump_symbol_regex)) {
			instruction_size = 5;
			location_counter += 3;
			output_code += "5";
			output_code += format_jump(code[0]);
			output_code += "f000";
			output_code += "0000";
			format_instruction_PC(code[1]);
			location_counter += 2;

			continue;
		}
		//	*	*	*	JUMP %SIMBOL	*	*	*
		else if (regex_match(line, jump_symbol_PC_regex)) {
			instruction_size = 5;
			location_counter += 3;
			output_code += "5";
			output_code += format_jump(code[0]);
			output_code += "f";
			output_code += "7";//pc
			output_code += "05";
			output_code += "0000";
			format_instruction_PC(code[1], true);
			location_counter += 2;
			continue;
		}
		//	*	*	*	JUMP *LITERAL	*	*	*
		else if (regex_match(line, jump_literal_MEM_regex)) {
			instruction_size = 5;
			location_counter += 5;
			output_code += "5";
			output_code += format_jump(code[0]);
			output_code += "f004";
			if (regex_match(code[1], decimal_regex)) {
				int a = stoi(code[1]);
				output_code += format_decimal(a);
			}
			else if (regex_match(code[1], hexadecimal_regex)) {
				int a = stoi(code[1], nullptr, 0);
				output_code += format_decimal(a);
			}
			else {
				cout << "***GRESKA*** na liniji " << line_number <<" - nepoznat literal" <<endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			continue;
		}
		//	*	*	*	JUMP *REG	*	*	*
		else if (regex_match(line, jump_reg_regex)) {
			instruction_size = 3;
			location_counter += 3;
			output_code += "5";
			output_code += format_jump(code[0]);
			output_code += "f";
			output_code += register_value(code[1]);
			output_code += "01";
			continue;
		}
		//	*	*	*	JUMP *SIMBOL	*	*	*
		else if (regex_match(line, jump_symbol_MEM_regex)) {
			instruction_size = 5;
			location_counter += 3;
			output_code += "5";
			output_code += format_jump(code[0]);
			output_code += "f004";
			output_code += "0000";
			format_instruction_PC(code[1]);
			location_counter += 2;
			continue;
		}
		//	*	*	*	JUMP *[REG]	*	*	*
		else if (regex_match(line, jump_reg_MEM_regex)) {
			instruction_size = 3;
			location_counter += 3;
			output_code += "5";
			output_code += format_jump(code[0]);
			output_code += "f";
			output_code += register_value(code[1]);
			output_code += "02";
			continue;
		}
		//	*	*	*	JUMP *[REG + L]	*	*	*
		else if (regex_match(line, jump_regIND_literal_regex)) {
			instruction_size = 5;
			location_counter += 5;
			output_code += "5";
			output_code += format_jump(code[0]);
			output_code += "f";
			output_code += register_value(code[1]);
			output_code += "03";	//proveriti
			if (regex_match(code[2], decimal_regex)) {
				int a = stoi(code[2]);
				output_code += format_decimal(a);
			}
			else if (regex_match(code[2], hexadecimal_regex)) {
				int a = stoi(code[2], nullptr, 0);
				output_code += format_decimal(a);
			}
			else {
				cout << "***GRESKA*** na liniji " << line_number << " - nepoznat literal"<<endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			continue;
		}
		//	*	*	*	JUMP *[REG + S]	*	*	*
		else if (regex_match(line, jump_regIND_symbol_regex)) {
			instruction_size = 5;
			location_counter += 3;
			output_code += "5";
			output_code += format_jump(code[0]);
			output_code += "f";
			output_code += register_value(code[1]);
			output_code += "03"; 
			output_code += "0000";
			format_instruction_PC(code[2]);
			location_counter += 2;
			continue;
		}
		//	*	*	*	PUSH	*	*	*
		else if (regex_match(line, push_regex)) {
			//str reg, sp - regind - predec
			instruction_size = 3;
			location_counter += 3;
			output_code += "b0";
			output_code += register_value(code[1]);
			output_code += register_value("sp");
			output_code += "1";
			output_code += "2";
			continue;
		}
		//	*	*	*	POP	*	*	*
		else if (regex_match(line, pop_regex)) {
			//ldr reg, sp - regind - postinc
			instruction_size = 3;
			location_counter += 3;
			output_code += "a0";
			output_code += register_value(code[1]);
			output_code += register_value("sp");
			output_code += "4";
			output_code += "2";
			continue;
		}
		//	*	*	*	XCHG	*	*	*
		else if (regex_match(line, xchg_regex)) {
			instruction_size = 2;
			location_counter += 2;
			output_code += "60";
			output_code += register_value(code[1]);
			output_code += register_value(code[2]);
			continue;
		}
		//	*	*	*	ARITMETIC	*	*	*
		else if (regex_match(line, aritmetic_regex)) {
			instruction_size = 2;
			location_counter += 2;
			output_code += "7";
			if (code[0] == "add") {
				output_code += "0";
			}
			else if (code[0] == "sub") {
				output_code += "1";
			}
			else if (code[0] == "mul") {
				output_code += "2";
			}
			else if (code[0] == "div") {
				output_code += "3";
			}
			else {
				cout << "***GRESKA*** na liniji " << line_number <<" - nije prepoznata aritmeticka operacija"<< endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			output_code += register_value(code[1]);
			output_code += register_value(code[2]);
			continue;
		}
		//	*	*	*	CMP	*	*	*
		else if (regex_match(line, cmp_regex)) {
			instruction_size = 2;
			location_counter += 2;
			output_code += "74";
			output_code += register_value(code[1]);
			output_code += register_value(code[2]);
			continue;
		}
		//	*	*	*	NOT	*	*	*
		else if (regex_match(line, not_regex)) {
			instruction_size = 2;
			location_counter += 2;
			output_code += "80";
			output_code += register_value(code[1]);
			output_code += "0";
			continue;
		}
		//	*	*	*	LOGIC	*	*	*
		else if (regex_match(line, logic_regex)) {
			instruction_size = 2;
			location_counter += 2;
			output_code += "8";
			if (code[0] == "and") {
				output_code += "1";
			}
			else if (code[0] == "or") {
				output_code += "2";
			}
			else if (code[0] == "xor") {
				output_code += "3";
			}
			else {
				//viska
				cout << "***GRESKA*** na liniji " << line_number << " - nije prepoznata logicka operacija" <<endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			output_code += register_value(code[1]);
			output_code += register_value(code[2]);
			continue;
		}
		//	*	*	*	TEST	*	*	*
		else if (regex_match(line, test_regex)) {
			instruction_size = 2;
			location_counter += 2;
			output_code += "84";
			output_code += register_value(code[1]);
			output_code += register_value(code[2]);
			continue;
		}
		//	*	*	*	SHIFT	*	*	*
		else if (regex_match(line, shift_regex)) {
			instruction_size = 2;
			location_counter += 2;
			output_code += "9";
			if (code[0] == "shl") {
				output_code += "0";
			}
			else if (code[0] == "shr") {
				output_code += "1";
			}
			else {
				//viska
				cout << "***GRESKA*** na liniji " << line_number <<" - nije prepoznata pomeracka operacija"<< endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			output_code += register_value(code[1]);
			output_code += register_value(code[2]);
			continue;
		}
		//	*	*	*	LS $LITERAL	*	*	*
		else if (regex_match(line, ldrstr_literal_regex)) {
			instruction_size = 5;
			location_counter += 5;
			if (code[0] == "ldr") {
				output_code += "a";
			}
			else {
				output_code += "b";
			}
			output_code += "0";
			output_code += register_value(code[1]);
			output_code += "000";
			if (regex_match(code[2], decimal_regex)) {
				int a = stoi(code[2]);
				output_code += format_decimal(a);
			}
			else if (regex_match(code[2], hexadecimal_regex)) {
				int a = stoi(code[2], nullptr, 0);
				output_code += format_decimal(a);
			}
			else {
				cout << "***GRESKA*** na liniji " << line_number <<" - nije prepoznat literal"<< endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			continue;
		}
		//	*	*	*	LS 	$SIMBOL	*	*	*
		else if (regex_match(line, ldrstr_symbol_regex)) {
			instruction_size = 5;
			location_counter += 3;
			if (code[0] == "ldr") {
				output_code += "a";
			}
			else {
				output_code += "b";
			}
			output_code += "0";
			output_code += register_value(code[1]);
			output_code += "000";
			output_code += "0000";
			format_instruction_PC(code[2]);
			location_counter += 2;
			continue;
		}
		//	*	*	*	LS 	LITERAL	*	*	*
		else if (regex_match(line, ldrstr_literal_MEM_regex)) {
			instruction_size = 5;
			location_counter += 5;
			if (code[0] == "ldr") {
				output_code += "a";
			}
			else {
				output_code += "b";
			}
			output_code += "0";
			output_code += register_value(code[1]);
			output_code += "004";
			if (regex_match(code[2], decimal_regex)) {
				int a = stoi(code[2]);
				output_code += format_decimal(a);
			}
			else if (regex_match(code[2], hexadecimal_regex)) {
				int a = stoi(code[2], nullptr, 0);
				output_code += format_decimal(a);
			}
			else {
				cout << "***GRESKA*** na liniji " << line_number <<" - nije prepoznat literal" << endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			continue;
		}
		//	*	*	*	LS 	SIMBOL	*	*	*
		else if (regex_match(line, ldrstr_symbol_MEM_APS_regex)) {
			instruction_size = 5;
			location_counter += 3;
			if (code[0] == "ldr") {
				output_code += "a";
			}
			else {
				output_code += "b";
			}
			output_code += "0";
			output_code += register_value(code[1]);
			output_code += "004";
			output_code += "0000";
			format_instruction_PC(code[2]);
			location_counter += 2;
			continue;
		}
		//	*	*	*	LS 	%SIMBOL	*	*	*
		else if (regex_match(line, ldrstr_symbol_PC_regex)) {
			instruction_size = 5;
			location_counter += 3;
			if (code[0] == "ldr") {
				output_code += "a";
			}
			else {
				output_code += "b";
			}
			output_code += "0";
			output_code += register_value(code[1]);
			output_code += register_value("pc");//r7
			output_code += "03";
			format_instruction_PC(code[2], true);
			location_counter += 2;
			continue;
		}
		//	*	*	*	LS 	REG	*	*	*
		else if (regex_match(line, ldrstr_reg_regex)) {
			instruction_size = 3;
			location_counter += 3;
			if (code[0] == "ldr") {
				output_code += "a";
			}
			else {
				output_code += "b";
			}
			output_code += "0";
			output_code += register_value(code[1]);
			output_code += register_value(code[2]);
			output_code += "01";
			continue;
		}
		//	*	*	*	LS 	[REG]	*	*	*
		else if (regex_match(line, ldrstr_reg_MEM_regex)) {
			instruction_size = 3;
			location_counter += 3;
			if (code[0] == "ldr") {
				output_code += "a";
			}
			else {
				output_code += "b";
			}
			output_code += "0";
			output_code += register_value(code[1]);
			output_code += register_value(code[2]);
			output_code += "02";
			continue;
		}
		//	*	*	*	LS 	[REG + L]	*	*	*
		else if (regex_match(line, ldrstr_regIND_literal_regex)) {
			instruction_size = 5;
			location_counter += 5;
			if (code[0] == "ldr") {
				output_code += "a";
			}
			else {
				output_code += "b";
			}
			output_code += "0";
			output_code += register_value(code[1]);
			output_code += register_value(code[2]);
			output_code += "03";
			if (regex_match(code[3], decimal_regex)) {
				int a = stoi(code[3]);
				output_code += format_decimal(a);
			}
			else if (regex_match(code[3], hexadecimal_regex)) {
				int a = stoi(code[3], nullptr, 0);
				output_code += format_decimal(a);
			}
			else {
				cout << "***GRESKA*** na liniji " << line_number << " - nije prepoznat literal" << endl;
				input_file.close();
				output_file.close();
				exit(-1);
				break;
			}
			continue;
		}
		//	*	*	*	LS 	[REG + S]	*	*	*
		else if (regex_match(line, ldrstr_regIND_symbol_regex)) {
			instruction_size = 5;
			location_counter += 3;
			if (code[0] == "ldr") {
				output_code += "a";
			}
			else {
				output_code += "b";
			}
			output_code += "0";
			output_code += register_value(code[1]);
			output_code += register_value(code[2]);
			output_code += "03";
			output_code += "0000";
			format_instruction_PC(code[3]);
			location_counter += 2;
			continue;
		}
		else {
			cout << "***GRESKA*** na liniji " << line_number << " - nepoznata instrukcija" << endl;
			input_file.close();
			output_file.close();
			exit(-1);
		}

	}
	if (run) {
		cout << "***GRESKA*** nije navedena instrukcija .end" << endl;
		input_file.close();
		output_file.close();
		exit(-1);
	}
	else {
		cout << endl << "while successfully ended" << endl;
	}
	bool bp = backPatch();
	if (bp) {
		cout << endl << "backPatch successfully ended" << endl;
		print();
	}
	else {
		cout << "***GRESKA***  prilikom backPatch" << endl;
		input_file.close();
		output_file.close();
		exit(-1);
	}
}

bool Assembler::backPatch() {

	symbol_table.set_size_section("UND", 0);
	for (Symbol_table_entry& s : symbol_table.get_symbol_list()) {

		if (s.get_type() == 'u') {
			if (s.get_name() == "UND")continue;
			cout << "***GRESKA*** nedefinisan simbol " << s.get_name() << endl;
			input_file.close();
			output_file.close();
			exit(-1);
			return false;
		}

		else if (s.get_type() == 'e') {

			if (s.get_patch_list().empty()) {
				cout << "*** .extern " << s.get_name() << " se ne koristi" << endl;
				//continue;
			}
			else {
				int b = symbol_table.get_number(s.get_name());
				for (Patch& p : s.get_patch_list()) {
					relocation_table.add(p.get_section(), p.get_location(), b, p.get_type(), p.get_isPC());
				}
			}
		}

		else if (s.get_type() == 'g' || s.get_type() == 'l') {
			int b = symbol_table.get_number(s.get_name());
			for (Patch& p : s.get_patch_list()) {
				if (p.get_type() == 'u') {
					cout << "postoji undefined" << endl;
					continue;
				}
				relocation_table.add(p.get_section(), p.get_location(), b, s.get_type(), p.get_isPC());
			}
		}
		else if (s.get_type() == 's') {
			//continue;
		}
		else {
			cout << "***GRESKA*** tip simbola " << s.get_name() << " nije prepoznat" << endl;
			input_file.close();
			output_file.close();
			exit(-1);
			return false;
			break;
		}
	}
	return true;
}
vector<string> Assembler::format_line(string line) {
	vector<string> lista;
	stringstream sline(line);
	do {
		string el;
		sline >> el;
		el.erase(remove(el.begin(), el.end(), '['), el.end());
		el.erase(remove(el.begin(), el.end(), ']'), el.end());
		el.erase(remove(el.begin(), el.end(), '+'), el.end());
		el.erase(remove(el.begin(), el.end(), ','), el.end());
		el.erase(remove(el.begin(), el.end(), '%'), el.end());
		el.erase(remove(el.begin(), el.end(), '*'), el.end());
		el.erase(remove(el.begin(), el.end(), '$'), el.end());
		if (!el.size())continue;
		lista.push_back(el);
	} while (sline);
	return lista;
}

void Assembler::print() {
	output_file << output_code << '\n' << endl;
	output_file << symbol_table << '\n' << endl;
	output_file << relocation_table << endl;
	//output_file << legenda;
}
string Assembler::register_value(string reg) {
	if (reg == "r0")
		return "0";
	else if (reg == "r1")
		return "1";
	else if (reg == "r2")
		return "2";
	else if (reg == "r3")
		return "3";
	else if (reg == "r4")
		return "4";
	else if (reg == "r5")
		return "5";
	else if (reg == "r6" || reg == "sp")
		return "6";
	else if (reg == "r7" || reg == "pc")
		return "7";
	else if (reg == "r8" || reg == "psw")//dodato
		return "8";
	else {
		cout << "***GRESKA*** postoji neprepoznata vrednost registra";
		input_file.close();
		output_file.close();
		exit(-1);
		return "";
	}
}

string Assembler::format_decimal(int dec) {
	stringstream ss;
	ss << std::hex << dec; // int decimal_value
	std::string res(ss.str());
	string a = "";
	if (res.size() == 1) {
		a.insert(0, "0");
		a.insert(1, res);
		a.insert(2, "00");
	}
	else if (res.size() == 2) {
		a.insert(0, res);
		a.insert(2, "00");
	}
	else if (res.size() == 3) {
		string pr = res.substr(0, 1);
		string dr = res.substr(1, 2);

		a.insert(0, dr);
		a.insert(2, "0");
		a.insert(3, pr);
	}
	else if (res.size() == 4) {
		string pr = res.substr(0, 2);
		string dr = res.substr(2, 2);
		a.insert(0, dr);
		a.insert(2, pr);
	}
	else {
		cout << "***GRESKA***  prevelik broj unet "<<endl;
		input_file.close();
		output_file.close();
		exit(-1);
	}
	return a;
}

void Assembler::format_instruction_PC(string name, bool pc) {
	bool exists = symbol_table.check_if_exists(name);
	if (exists) {
		symbol_table.add_patch(name, current_section, location_counter, pc);
	}
	else {
		symbol_table.add(name, current_section, location_counter);
		symbol_table.add_patch(name, current_section, location_counter, pc);
	}
}
string Assembler::format_jump(string name) {
	if (name == "jmp") {
		return"0";
	}
	else if (name == "jeq") {
		return"1";
	}
	else if (name == "jne") {
		return"2";
	}
	else if (name == "jgt") {
		return"3";
	}
	else {
		cout << "***GRESKA*** neprepoznata instrukcija skoka" << endl;
		input_file.close();
		output_file.close();
		exit(-1);
	}
}
