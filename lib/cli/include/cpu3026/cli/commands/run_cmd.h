#pragma once
#ifndef CPU3026_CLI_COMMANDS_RUN_CMD_H
#define CPU3026_CLI_COMMANDS_RUN_CMD_H

#include <iostream>
#include <fstream>
#include <cpu3026/vm/vm.h>

namespace cpu3026::commands {
	//loads bin file into memory
	int run_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
		vm.step(100);
		return 0;
	}
}

#endif // CPU3026_CLI_COMMANDS_RUN_CMD_H
