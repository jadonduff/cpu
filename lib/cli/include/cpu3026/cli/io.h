#pragma
#ifndef CPU3026_CLI_COMMANDS_IO_H
#define CPU3026_CLI_COMMANDS_IO_H

#include "commands.h"
#include <iostream>
#include <string>

namespace elements {
	class game_io {
		std::istream* cin;
		std::ostream* cout;
		command_handler cmd;
	public:
		game_io(std::istream* cin, std::ostream* cout) : cin(cin), cout(cout), cmd(default_commands()) {}

		//blocking function for std::cin
		bool cmd_in() {
			*cout << "$ ";
			std::string line;
			std::getline(*cin, line);
			if (line.empty()) return true;
			int ret_code = cmd.invoke(line, *cin, *cout);

			if (ret_code != 0) {
				std::cout << "return code: " << std::hex << ret_code << std::endl;
			}

			return true;
		}
	};
}

#endif // CPU3026_CLI_COMMANDS_IO_H