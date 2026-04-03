#pragma once
#ifndef CPU3026_CLI_COMMANDS_LOAD_CMD_H
#define CPU3026_CLI_COMMANDS_LOAD_CMD_H

#include <iostream>
#include <fstream>
#include <cpu3026/vm/vm.h>

namespace cpu3026::commands {
	//loads bin file into memory
	int load_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
		if(argc != 2) {
			cout << "Usage: load <filename>" << std::endl;
			return 1;
		}

		std::ifstream file(argv[1], std::ios::binary);

		if(!file) {
			cout << "Failed to open file: " << argv[1] << std::endl;
			return 2;
		}

		std::vector<byte_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		if (buffer.size() > cpu3026::ROM_SIZE) {
			cout << "binary file is too large to fit into memory:" << buffer.size() << ". max size is " << cpu3026::ROM_SIZE << " bytes." << std::endl;
		}

		vm.memory()->load_rom(buffer);

		return 0;
	}
}

#endif CPU3026_CLI_COMMANDS_LOAD_CMD_H
