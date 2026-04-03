#pragma
#ifndef CPU3026_CLI_COMMANDS_IO_H
#define CPU3026_CLI_COMMANDS_IO_H

#include "commands.h"
#include "command_handler.h"
#include <iostream>
#include <string>
#include <cpu3026/vm/vm.h>

namespace cpu3026 {
	class project_io {
		std::istream* cin;
		std::ostream* cout;
		vm_command_handler cmd;
	public:
		project_io(std::istream* cin, std::ostream* cout) : cin(cin), cout(cout), cmd(default_commands()) {}

		//blocking function for std::cin
		bool cmd_in(virtual_machine& vm) {
			if (!cin || !cout) return false;
			*cout << "$ ";
			std::string line;
			std::getline(*cin, line);
			if (line.empty()) return true;
			int ret_code = cmd.invoke(line, vm, *cin, *cout);

			if (ret_code != 0) {
				std::cout << "return code: " << std::hex << ret_code << std::endl;
			}

			return true;
		}
	};
}

#endif // CPU3026_CLI_COMMANDS_IO_H