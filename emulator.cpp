#include "emulator.h"
#include <iomanip>

const regex he_format_regex("^.*\.(hex)$");
const regex line_format_regex("^([0-9a-f]{4})(:)([\\s]*[a-fA-F0-9]{2})*([\\s]*)$");

Emulator::Emulator(string in_file/*, string out_file*/) {
	this->input_file_name = in_file;
	if (!regex_match(in_file, he_format_regex)) {
		cout << "***GRESKA*** ulazni fajl nije u dobrom formatu" << endl;
		exit(-1);
	}

	this->input_file.open(input_file_name);
	this->memory_cap = 65535;	//2^16 B
	this->memory = new string[memory_cap];
	registers[6] = memory_cap - 1; //sp

	if (!input_file.is_open()) {
		cout << "***GRESKA*** nije otvoren ulazni fajl" << endl;
		exit(-1);
		//error
	}
	this->output_file.open("emulator_exit.o");
}

void Emulator::emulate() {
	string line = "";
	int curr = 0;
	while (getline(input_file, line)) {
		if (line.empty()) {
			break;
		}
		if (!regex_match(line, line_format_regex)) {
			cout << "***GRESKA*** los format ulaznog fajla" << endl;
			input_file.close();
			exit(-1);
			break;
		}

		vector<string> lista;
		stringstream sline(line);
		do {
			string el;
			sline >> el;
			if (!el.size())continue;
			lista.push_back(el);
		} while (sline);

		int len = lista.size();
		for (int i = 1; i < len; i++) {
			if (!lista[i].empty()) {
				memory[curr++] = lista[i];
			}
		}
		if (curr >= memory_cap) {
			cout << "***GRESKA***	memorijski prostor prevelik " << endl;
			input_file.close();
			exit(-1);
		}
	}

	//pocetna adresa
	reset();
	
	while (run) {
		string instruction = get_byte(registers[7]++);

		//	*	*	*	HALT	*	*	*
		if (instruction == "00") {
			registers[7]--;
			run = false;
			exit_print();
		}
		//	*	*	*	INT		*	*	*
		else if (instruction == "10") {
			string RegsDescr = get_byte(registers[7]++);
			push("pc");
			push("psw");
			int reg_num = get_high_4_bits(RegsDescr);
			string ret = "";
			ret += memory[(registers[reg_num] % 8) * 2 + 1];
			ret += memory[(registers[reg_num] % 8) * 2];
			int b = convert_hexa_to_dec(ret);
			registers[7] = b;
		}
		//	*	*	*	IRET	*	*	*
		else if (instruction == "20") {
			pop("psw");
			pop("pc");
		}
		//	*	*	*	CALL	*	*	*
		else if (instruction == "30") {
			string RegsDescr = get_byte(registers[7]++);
			string AddrMode = get_byte(registers[7]++);
			int reg_num = get_low_4_bits(RegsDescr);
			int addr_update = get_high_4_bits(AddrMode);
			int addr_type = get_low_4_bits(AddrMode);
			if (addr_type == 0) {//imm
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int next = convert_hexa_to_dec(adr);
				push("pc");
				registers[7] = next;
			}
			else if (addr_type == 1) {//regdir
				push("pc");
				registers[7] = registers[reg_num];
			}
			else if (addr_type == 2) { //regind
				push("pc");
				//update register bits
				if (addr_update == 0) {
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 1) {
					registers[reg_num] -= 2;
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 2) {
					registers[reg_num] += 2;
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 3) {
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
					registers[reg_num] -= 2;
				}
				else if (addr_update == 4) {
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
					registers[reg_num] += 2;
				}
				else {
					cout << "***nekorektna vrednost update registara***  pc: " << registers[7] - 3 << endl;
					err(1);
				}
			}
			else if (addr_type == 3) {//regind16
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int pom = convert_hexa_to_dec(adr);
				push("pc");

				//update register bits
				if (addr_update == 0) {
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 1) {
					registers[reg_num] -= 2;
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 2) {
					registers[reg_num] += 2;
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 3) {
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
					registers[reg_num] -= 2;
				}
				else if (addr_update == 4) {
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
					registers[reg_num] += 2;
				}
				else {
					cout << "***nekorektna vrednost update registara***  pc: " << registers[7] - 3 << endl;
					err(1);
				}
			}
			else if (addr_type == 4) {//mem
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int adr_op = convert_hexa_to_dec(adr);
				push("pc");
				string low = get_byte(adr_op);
				string high = get_byte(adr_op + 1);
				string op = high+low;
				int next = convert_hexa_to_dec(op);
				registers[7] = next;

			}
			else if (addr_type == 5) {//regdir16
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int pom = convert_hexa_to_dec(adr);
				push("pc");
				registers[7] = registers[reg_num] + pom - 2;//-2 zbog +2 kod dohvatanja dataH i dataL
			}
			else {
			cout << "***nekorektan nacin adresiranja***  pc: " << registers[7] - 3 << endl;
			err(1);
			}
		}
		//	*	*	*	RET	*	*	*
		else if (instruction == "40") {
			pop("pc");
		}
		//	*	*	*	JMP - JEQ - JNE - JGT	*	*	*
		else if (instruction == "50" || instruction == "51" || instruction == "52" || instruction == "53") {
			string RegsDescr = get_byte(registers[7]++);
			string AddrMode = get_byte(registers[7]++);
			int reg_num = get_low_4_bits(RegsDescr);
			int addr_update = get_high_4_bits(AddrMode);
			int addr_type = get_low_4_bits(AddrMode);
			if (instruction == "51") {
				//jeq
				int z = get_Z();
				if (z == 1) {
					//
				}
				else {
					if (addr_type == 0 || addr_type == 3 || addr_type == 4 || addr_type == 5) {
						registers[7] += 2;
					}
					continue;
				}
			}
			else if (instruction == "52") {
				//jne
				int z = get_Z();
				if (z == 0) {
					//
				}
				else {
					if (addr_type == 0 || addr_type == 3 || addr_type == 4 || addr_type == 5) {
						registers[7] += 2;
					}
					continue;
				}
			}
			else if (instruction == "53") {
				//jgt
				int n = get_N();
				if (n == 0) {
					//
				}
				else {
					if (addr_type == 0 || addr_type == 3 || addr_type == 4 || addr_type == 5) {
						registers[7] += 2;
					}
					continue;
				}
			}

			if (addr_type == 0) {//imm
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int next = convert_hexa_to_dec(adr);
				registers[7] = next;
			}
			else if (addr_type == 1) {//regdir
				registers[7] = registers[reg_num];
			}
			else if (addr_type == 2) { //regind
				//update register bits
				if (addr_update == 0) {
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 1) {
					registers[reg_num] -= 2;
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 2) {
					registers[reg_num] += 2;
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 3) {
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
					registers[reg_num] -= 2;
				}
				else if (addr_update == 4) {
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
					registers[reg_num] += 2;
				}
				else {
					cout << "***nekorektna vrednost update registara***  pc: " << registers[7] - 3 << endl;
					err(1);
				}
			}
			else if (addr_type == 3) {//regind16
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int pom = convert_hexa_to_dec(adr);
				//update register bits
				if (addr_update == 0) {
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 1) {
					registers[reg_num] -= 2;
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 2) {
					registers[reg_num] += 2;
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
				}
				else if (addr_update == 3) {
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
					registers[reg_num] -= 2;
				}
				else if (addr_update == 4) {
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					registers[7] = next;
					registers[reg_num] += 2;
				}
				else {
					cout << "***nekorektna vrednost update registara***  pc: " << registers[7] - 3 << endl;
					err(1);
				}
			}
			else if (addr_type == 4) {//mem
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int adr_op = convert_hexa_to_dec(adr);
				string low = get_byte(adr_op);
				string high = get_byte(adr_op + 1);
				string op = high+low;
				int next = convert_hexa_to_dec(op);
				registers[7] = next;

			}
			else if (addr_type == 5) {//regdir16
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int pom = convert_hexa_to_dec(adr);
				registers[7] = registers[reg_num] + pom - 2;//-2 zbog +2 kod dohvatanja dataH i dataL
			}
			else {
			cout << "***nekorektan nacin adresiranja***  pc: " << registers[7] - 3 << endl;
			err(1);
			}
		}
		//	*	*	*	XCHG	*	*	*
		else if (instruction == "60") {
			int i = get_I();
			set_I(0);
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			unsigned short temp = registers[reg_d];
			registers[reg_d] = registers[reg_s];
			registers[reg_s] = temp;
			set_I(i);
		}
		//	*	*	*	ADD	*	*	*
		else if (instruction == "70") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			registers[reg_d] = registers[reg_d] + registers[reg_s];
		}
		//	*	*	*	SUB	*	*	*
		else if (instruction == "71") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			registers[reg_d] = registers[reg_d] - registers[reg_s];
		}
		//	*	*	*	MUL	*	*	*
		else if (instruction == "72") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			registers[reg_d] = registers[reg_d] * registers[reg_s];
		}
		//	*	*	*	DIV	*	*	*
		else if (instruction == "73") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			registers[reg_d] = registers[reg_d] / registers[reg_s];
		}
		//	*	*	*	CMP	*	*	*
		else if (instruction == "74") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			short res = registers[reg_d] - registers[reg_s];
			bool isNegative = (0x0000 & res) != 0;
			if (registers[reg_d] > 0 && registers[reg_s] > 0 && isNegative) {
				set_O(1);
			}else if (registers[reg_d] < 0 && registers[reg_s] < 0 && !isNegative) {
				set_O(1);
			}
			else {
				set_O(0);
			}
			if (registers[reg_d] - registers[reg_s] == 0) {
				set_Z(1);
			}
			else {
				set_Z(0);
			}
			if (isNegative) {
				set_N(1);
			}
			else {
				set_N(0);
			}
		}
		//	*	*	*	NOT	*	*	*
		else if (instruction == "80") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			registers[reg_d] = ~registers[reg_d];
		}
		//	*	*	*	AND	*	*	*
		else if (instruction == "81") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			registers[reg_d] = registers[reg_d] & registers[reg_s];
		}
		//	*	*	*	OR	*	*	*
		else if (instruction == "82") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			registers[reg_d] = registers[reg_d] | registers[reg_s];
		}
		//	*	*	*	XOR	*	*	*
		else if (instruction == "83") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			registers[reg_d] = registers[reg_d] ^ registers[reg_s];
		}
		//	*	*	*	TEST	*	*	*
		else if (instruction == "84") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			int num = registers[reg_d] & registers[reg_s];
			if (num == 0) {
				set_Z(1);
			}
			else {
				set_Z(0);
			}
			int k = 15;
			int mask = 1 << k;
			int masked_n = num & mask;
			int ret = masked_n >> k;
			if (ret == 1) {
				set_N(1);
			}
			else {
				set_N(0);
			}
		}
		//	*	*	*	SHL	*	*	*
		else if (instruction == "90") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			if (registers[reg_s] < 0) {
				cout << "***greska prilikom shift left*** negativan broj - pc: " << registers[7]-1 << endl;
				err(1);
			}
			short t = registers[reg_d];
			registers[reg_d] <<= registers[reg_s];
			if (registers[reg_d] == 0) {
				set_Z(1);
			}
			else {
				set_Z(0);
			}
			int k = 15;
			int mask = 1 << k;
			int masked_n = registers[reg_d] & mask;
			int ret = masked_n >> k;
			if (ret == 1) {
				set_N(1);
			}
			else {
				set_N(0);
			}
			t >>= (16 - registers[reg_s]);
			if (t&0x0001) {
				set_C(1);
			}
			else{
				set_C(0);
			}
		}
		//	*	*	*	SHR	*	*	*
		else if (instruction == "91") {
			string RegsDescr = get_byte(registers[7]++);
			int reg_d = get_high_4_bits(RegsDescr);
			int reg_s = get_low_4_bits(RegsDescr);
			if (registers[reg_s] < 0) {
				cout << "***greska prilikom shift right*** negativan broj - pc: " << registers[7] - 1 << endl;
				err(1);
			}
			short t = registers[reg_d];
			registers[reg_d] >>= registers[reg_s];
			if (registers[reg_d] == 0) {
				set_Z(1);
			}
			else {
				set_Z(0);
			}
			int k = 15;
			int mask = 1 << k;
			int masked_n = registers[reg_d] & mask;
			int ret = masked_n >> k;
			if (ret == 1) {
				set_N(1);
			}
			else {
				set_N(0);
			}
			t <<= (16 - registers[reg_s]);
			if (t & 0x8000) {
				set_C(1);
			}
			else {
				set_C(0);
			}
		}
		//	*	*	*	LDR-POP		*	STR-PUSH	*	*	*
		else if (instruction == "a0" || instruction == "b0") {
			string RegsDescr = get_byte(registers[7]++);
			string AddrMode = get_byte(registers[7]++);
			int reg_num = get_low_4_bits(RegsDescr);
			int addr_update = get_high_4_bits(AddrMode);
			int addr_type = get_low_4_bits(AddrMode);
			int reg_d = get_high_4_bits(RegsDescr);
			if (instruction == "a0" && reg_num == 6 && addr_update == 4 && addr_type == 2) {
				//pop
				pop(reg_d);
				print_curr(instruction);
				continue;
			}
			if (instruction == "b0" && reg_num == 6 && addr_update == 1 && addr_type == 2) {
				//push
				push(reg_d);
				print_curr(instruction);
				continue;
			}
			if (addr_type == 0) {//imm
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int next = convert_hexa_to_dec(adr);
				if (instruction == "a0") {
					registers[reg_d] = next;
				}
				else {
					string a = convert_dec_to_hexa(registers[reg_d]);
					set_2_bytes_le(next, a);
				}
			}
			else if (addr_type == 1) {//regdir
				if (instruction == "a0") {
					registers[reg_d] = registers[reg_num];
				}
				else {
					registers[reg_num] = registers[reg_d];
				}
			}
			else if (addr_type == 2) { //regind
				//update register bits
				if (addr_update == 0) {
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high + low;
					int next = convert_hexa_to_dec(adr);
					if (instruction == "a0") {
						registers[reg_d] = next;
					}
					else {
						string a = convert_dec_to_hexa(registers[reg_d]);
						set_2_bytes_le(registers[reg_num], a);
					}
				}
				else if (addr_update == 1) {
					registers[reg_num] -= 2;
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					if (instruction == "a0") {
						registers[reg_d] = next;
					}
					else {
						string a = convert_dec_to_hexa(registers[reg_d]);
						set_2_bytes_le(next, a);
					}
				}
				else if (addr_update == 2) {
					registers[reg_num] += 2;
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					if (instruction == "a0") {
						registers[reg_d] = next;
					}
					else {
						string a = convert_dec_to_hexa(registers[reg_d]);
						set_2_bytes_le(next, a);
					}
				}
				else if (addr_update == 3) {
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					if (instruction == "a0") {
						registers[reg_d] = next;
					}
					else {
						string a = convert_dec_to_hexa(registers[reg_d]);
						set_2_bytes_le(next, a);
					}
					registers[reg_num] -= 2;
				}
				else if (addr_update == 4) {
					string low = get_byte(registers[reg_num]);
					string high = get_byte(registers[reg_num] + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					if (instruction == "a0") {
						registers[reg_d] = next;
					}
					else {
						string a = convert_dec_to_hexa(registers[reg_d]);
						set_2_bytes_le(next, a);
					}
					registers[reg_num] += 2;
				}
				else {
					cout << "***nekorektna vrednost update registara***  pc: " << registers[7] - 3 << endl;
					err(1);
				}
			}
			else if (addr_type == 3) {//regind16
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int pom = convert_hexa_to_dec(adr);
				//update register bits
				if (addr_update == 0) {
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					registers[reg_d] = next;
				}
				else if (addr_update == 1) {
					registers[reg_num] -= 2;
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					if (instruction == "a0") {
						registers[reg_d] = next;
					}
					else {
						string a = convert_dec_to_hexa(registers[reg_d]);
						set_2_bytes_le(next, a);
					}
				}
				else if (addr_update == 2) {
					registers[reg_num] += 2;
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					if (instruction == "a0") {
						registers[reg_d] = next;
					}
					else {
						string a = convert_dec_to_hexa(registers[reg_d]);
						set_2_bytes_le(next, a);
					}
				}
				else if (addr_update == 3) {
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					//mozda zameniti mesta??
					if (instruction == "a0") {
						registers[reg_d] = next;
					}
					else {
						string a = convert_dec_to_hexa(registers[reg_d]);
						set_2_bytes_le(next, a);
					}
					registers[reg_num] -= 2;
				}
				else if (addr_update == 4) {
					string low = get_byte(registers[reg_num] + pom);
					string high = get_byte(registers[reg_num] + pom + 1);
					string adr = high+low;
					int next = convert_hexa_to_dec(adr);
					if (instruction == "a0") {
						registers[reg_d] = next;
					}
					else {
						string a = convert_dec_to_hexa(registers[reg_d]);
						set_2_bytes_le(next, a);
					}
					registers[reg_num] += 2;
				}
				else {
					cout << "***nekorektna vrednost update registara***  pc: " << registers[7] - 3 << endl;
					err(1);
				}
			}
			else if (addr_type == 4) {//mem
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int adr_op = convert_hexa_to_dec(adr);
				string low = get_byte(adr_op);
				string high = get_byte(adr_op + 1);
				string op = high + low;
				int next = convert_hexa_to_dec(op);
				if (instruction == "a0") {
					registers[reg_d] = next;
				}
				else {
					string a = convert_dec_to_hexa(registers[reg_d]);
					set_2_bytes_le(adr_op, a);
				}

			}
			else if (addr_type == 5) {//regdir16
				string data_high = get_byte(registers[7]++);
				string data_low = get_byte(registers[7]++);
				string adr = data_low + data_high;
				int pom = convert_hexa_to_dec(adr);
				if (instruction == "a0") {
					registers[reg_d] = registers[reg_num] + pom;
				}
				else {
					string a = convert_dec_to_hexa(registers[reg_d]);
					set_2_bytes_le(registers[reg_num] + pom, a);
				}

			}
			else {
			cout << "***nekorektan nacin adresiranja***  pc: " << registers[7] - 3 << endl;
			err(1);
			}



		}
		else {
		cout << "***nekorektna instrukcija***  pc: " << registers[7] - 1 << endl;
		err(1);
		}
		print_curr(instruction);
		//
		if (get_I()) {
			continue;
		}
		else {
			//if timer interrupt
		}



	}//while run
}

string Emulator::get_byte(int pos) {
	string ret = "";
	ret = memory[pos];
	return ret;
}
void Emulator::set_byte(int pos, string byte) {
	memory[pos] = byte;
}
void Emulator::set_2_bytes_le(int pos, string byte) {
	string high = "";
	high += byte[0];
	high += byte[1];
	string low = "";
	low += byte[2];
	low += byte[3];
	memory[pos] = low;
	memory[pos + 1] = high;
}
string Emulator::get_2_bytes(int pos) {
	string ret = "";
	ret += memory[pos];
	ret += memory[pos + 1];
	return ret;
}
string Emulator::get_2_bytes_little_endian(int pos) {
	string ret = "";
	ret += memory[pos + 1];
	ret += memory[pos];
	return ret;
}
int Emulator::convert_hexa_to_dec(string hexa) {
	int res;
	stringstream ss;
	ss << std::hex << hexa;
	ss >> res;
	if (hexa[0] == 'a' || hexa[0] == 'b' || hexa[0] == 'c' || hexa[0] == 'd' || hexa[0] == 'e' || hexa[0] == 'f' || hexa[0] == '8' || hexa[0] == '9') {
		if (hexa.length() != 2) {
			res = ~res;
			res += 65536;
			res++;
			res = 0 - res;
		}
	}
	output_file << "hexa: " << hexa << " to dec: " << res << endl;
	return res;
}
string Emulator::convert_dec_to_hexa(int dec) {
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
		cout << "convert " << dec << " to hex:  " << result << endl;
		return result;
	}
}
void Emulator::push(string reg) {
	int reg_num = register_value(reg);
	if (reg_num < 0 || reg_num>8) {
		cout << "***losa vrednost registra*** push " << reg << endl;
		err(-1);
	}
	string bytes = convert_dec_to_hexa(registers[reg_num]);
	string high = "";
	string low = "";
	high += bytes[0];
	high += bytes[1];
	low += bytes[2];
	low += bytes[3];
	memory[--registers[6]] = high;
	memory[--registers[6]] = low;
}
void Emulator::push(int reg) {
	if (reg < 0 || reg>8) {
		cout << "***losa vrednost registra*** push "<<reg<<endl;
		err(-1);
	}
	string bytes = convert_dec_to_hexa(registers[reg]);
	string high = "";
	string low = "";
	high += bytes[0];
	high += bytes[1];
	low += bytes[2];
	low += bytes[3];
	memory[--registers[6]] = high;
	memory[--registers[6]] = low;
}
void Emulator::pop(string reg) {
	string ret = "";
	if (registers[6] + 2 > memory_cap) {
		cout << "***stek prazan, ne moze se skinuti***" << endl;
		err(-1);
	}
	int reg_num = register_value(reg);
	if (reg_num < 0 || reg_num>8) {
		cout << "***losa vrednost registra*** pop "<<reg<<endl;
		err(-1);
	}
	ret += memory[registers[6] + 1];
	ret += memory[registers[6]];
	registers[6] += 2;
	
	int b = convert_hexa_to_dec(ret);
	registers[reg_num] = b;
}
void Emulator::pop(int reg) {
	if (registers[6] + 2 > memory_cap) {
		cout << "***stek prazan, ne moze se skinuti***" << endl;
		err(-1);
	}
	if (reg < 0 || reg>8) {
		cout << "***losa vrednost registra*** pop " << reg << endl;
		err(-1);
	}
	string ret = "";
	ret += memory[registers[6] + 1];
	ret += memory[registers[6]];
	registers[6] += 2;
	int b = convert_hexa_to_dec(ret);
	registers[reg] = b;
}
int Emulator::register_value(string reg) {
	if (reg == "r0")
		return 0;
	else if (reg == "r1")
		return 1;
	else if (reg == "r2")
		return 2;
	else if (reg == "r3")
		return 3;
	else if (reg == "r4")
		return 4;
	else if (reg == "r5")
		return 5;
	else if (reg == "r6" || reg == "sp")
		return 6;
	else if (reg == "r7" || reg == "pc")
		return 7;
	else if (reg == "r8" || reg == "psw")	// proveriti ovo
		return 8;
	else {
		cout << "***losa vrednost registra*** "<<reg<<endl;
		err(-1);
	}
}

int Emulator::get_I() {
	int num = registers[8];
	int k = 15;
	int mask = 1 << k;
	int masked_n = num & mask;
	int thebit = masked_n >> k;
	return thebit;
}
int Emulator::get_N() {
	int num = registers[8];
	int k = 3;
	int mask = 1 << k;
	int masked_n = num & mask;
	int thebit = masked_n >> k;
	return thebit;
}
int Emulator::get_C() {
	int num = registers[8];
	int k = 2;
	int mask = 1 << k;
	int masked_n = num & mask;
	int thebit = masked_n >> k;
	return thebit;
}
int Emulator::get_O() {
	int num = registers[8];
	int k = 1;
	int mask = 1 << k;
	int masked_n = num & mask;
	int thebit = masked_n >> k;
	return thebit;
}
int Emulator::get_Z() {
	int num = registers[8];
	int k = 0;
	int mask = 1 << k;
	int masked_n = num & mask;
	int thebit = masked_n >> k;
	return thebit;
}
void Emulator::set_I(int num) {
	registers[8] |= num << 15;
}
void Emulator::set_N(int num) {
	registers[8] |= num << 3;
}
void Emulator::set_C(int num) {
	registers[8] |= num << 2;
}
void Emulator::set_O(int num) {
	registers[8] |= num << 1;
}
void Emulator::set_Z(int num) {
	registers[8] |= num << 0;
}
int Emulator::get_high_4_bits(string str) {
	int num = convert_hexa_to_dec(str);
	int k = 4;
	int mask = 15 << k;
	int masked_n = num & mask;
	int ret = masked_n >> k;
	return ret;
}
int Emulator::get_low_4_bits(string str) {
	int num = convert_hexa_to_dec(str);
	int k = 0;
	int mask = 15 << k;
	int masked_n = num & mask;
	int ret = masked_n >> k;
	return ret;
}


void Emulator::print_curr(string instruction) {
	output_file << endl << "NO: " << print_num << endl;
	print_num++;
	if (print_num > 90) {
		exit(0);
	}
	//	*	*	*	HALT	*	*	*
	if (instruction == "00") {
		output_file << " halt " << endl;
	}
	//	*	*	*	INT		*	*	*
	else if (instruction == "10") {
		output_file << " int " << endl;
	}
	//	*	*	*	IRET	*	*	*
	else if (instruction == "20") {
		output_file << " iret " << endl;
	}
	//	*	*	*	CALL	*	*	*
	else if (instruction == "30") {
		output_file << " call " << endl;
	}
	//	*	*	*	RET	*	*	*
	else if (instruction == "40") {
		output_file << " ret " << endl;
	}
	//	*	*	*	JMP	*	*	*
	else if (instruction == "50") {
		output_file << "jmp" << endl;
	}
	//	*	*	*	JEQ	*	*	*
	else if (instruction == "51") {
		output_file << " jeq " << endl;
	}
	//	*	*	*	JNE	*	*	*
	else if (instruction == "52") {
		output_file << " jne " << endl;
	}
	//	*	*	*	JGT	*	*	*
	else if (instruction == "53") {
		output_file << " jgt " << endl;
	}
	//	*	*	*	XCHG	*	*	*
	else if (instruction == "60") {
		output_file << " xchg " << endl;
	}
	//	*	*	*	ADD	*	*	*
	else if (instruction == "70") {
		output_file << " add " << endl;
	}
	//	*	*	*	SUB	*	*	*
	else if (instruction == "71") {
		output_file << " sub " << endl;
	}
	//	*	*	*	MUL	*	*	*
	else if (instruction == "72") {
		output_file << " mul " << endl;
	}
	//	*	*	*	DIV	*	*	*
	else if (instruction == "73") {
		output_file << " div " << endl;
	}
	//	*	*	*	CMP	*	*	*
	else if (instruction == "74") {
		output_file << " cmp " << endl;
	}
	//	*	*	*	NOT	*	*	*
	else if (instruction == "80") {
		output_file << " not " << endl;
	}
	//	*	*	*	AND	*	*	*
	else if (instruction == "81") {
		output_file << " and " << endl;
	}
	//	*	*	*	OR	*	*	*
	else if (instruction == "82") {
		output_file << " or " << endl;
	}
	//	*	*	*	XOR	*	*	*
	else if (instruction == "83") {
		output_file << " xor " << endl;
	}
	//	*	*	*	TEST	*	*	*
	else if (instruction == "84") {
		output_file << " test " << endl;
	}
	//	*	*	*	SHL	*	*	*
	else if (instruction == "90") {
		output_file << " shl " << endl;
	}
	//	*	*	*	SHR	*	*	*
	else if (instruction == "91") {
		output_file << " shr " << endl;
	}
	//	*	*	*	LDR		*	POP	*	*	*
	else if (instruction == "a0") {
		output_file << " ldr - pop " << endl;
	}
	//	*	*	*	STR		*	PUSH	*	*	*
	else if (instruction == "b0") {
		output_file << " str - push " << endl;
	}
	else {
		output_file << " xxxxxxx" << endl;
	}

	for (int i = 0; i < 6; i++) {
		output_file << ":" << registers[i];
		//cout << /*i <<*/ " : " << registers[i];
	}


	output_file << endl;
	output_file << "sp: " << registers[6] << endl;
	output_file << "pc: " << registers[7] << endl;
	output_file << "psw: " << registers[8] << endl;

	int z = 0;
	int r = 1;
	for (int i = 0; i < 353; i++) {
		if (z == 0) {
			output_file << r << "  ";
			if (r < 10)output_file << " ";
			r++;
		}output_file << get_byte(i) << " ";
		z++;
		if (z == 8) {
			z = 0;
			output_file << endl;
		}
	}
	output_file << endl << endl << "stack:" << endl;
	for (int i = 65265; i < 65280; i++) {
		output_file << setw(10) << i;
	}
	output_file << endl;
	for (int i = 65265; i < 65280; i++) {
		output_file << setw(10) << get_byte(i);
	}


	output_file << endl << "------------------------------------------------------------" << endl;
}

Emulator::~Emulator() {
	input_file.close();
	output_file.close();
}

void Emulator::reset() {
	curr_address = get_2_bytes_little_endian(0);
	int a = convert_hexa_to_dec(curr_address);
	registers[7] = a;// pc = a;
}

void Emulator::err(int intr) {
	if (intr == 1) {
		//nekorektna instrukcija
		push("pc");
		push("psw");
		curr_address = get_2_bytes_little_endian(2);
		int a = convert_hexa_to_dec(curr_address);
		registers[7] = a;// pc = a;
	}
	else if (intr == 2) {
		//timer
	}
	else if (intr == 3) {
		//terminal
	}
	else {

	}
	
}

void Emulator::exit_print() {
	cout << endl << endl << endl;
	cout << setw(48) << setfill('-') << "-" << endl;
	cout << "Emulated processor executed halt instruction" << endl;
	cout << "Emulated processor state: psw=0b";
	for (int i = 0; i < 16; i++) {
		int num = registers[8];
		int k = i;
		int mask = 1 << k;
		int masked_n = num & mask;
		int thebit = masked_n >> k;
		cout << thebit;
	}
	cout << endl;
	for (int r = 0; r < 8; r++) {
		string a = convert_dec_to_hexa(registers[r]);
		cout << "r" << r << "=0x" << a << "    ";
		if (r == 3) {
			cout << endl;
		}
	}
	cout << endl << endl << endl << endl << endl;
}