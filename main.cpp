#include <iostream>
#include <map>
#include <bitset>
#include <regex>
#include "asembler.h"
#include "linker.h"
#include "emulator.h"

using namespace std;

int main(int argc, char* argv[]) {
	string zad = argv[1];
	if (zad == "asembler") {
		string z = argv[2];
		if (z == "-o") {
			string input = argv[4];
			string output = argv[3];
			Assembler assembler(input, output);
			assembler.assemble();
		}
		else {
			cout << "***los unos argumenata***" << endl;
		}
	}
	else if (zad == "emulator") {
		string z = argv[2];
		//
		//
		//if .hex
		//
		//
		Emulator emulator(z);
		emulator.emulate();
	}
	else if (zad == "linker") {

		string z = argv[2];
		if (z == "-hex") {
			string zo = argv[3];
			if (zo == "-o") {
				string output = argv[4];
				vector<string> linker_input_list;
				for (int i = 5; i < argc; i++) {
					linker_input_list.push_back(argv[i]);

				}
				Linker linker(linker_input_list, output);
				linker.link();
			}
			else {
				cout << "***nije definisan izlazni fajl***" << endl;
			}
		}
		else {
			cout << "***obavezan -hex***" << endl;
		}
	}
	else {
		cout << "***nepoznata komanda***" << endl;
		return -1;
	}

	return 0;
}