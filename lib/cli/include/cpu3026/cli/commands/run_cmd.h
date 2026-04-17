#pragma once
#ifndef CPU3026_CLI_COMMANDS_RUN_CMD_H
#define CPU3026_CLI_COMMANDS_RUN_CMD_H

#include <iostream>
#include <fstream>
#include <cpu3026/vm/vm.h>
#include <vector>
#include <algorithm>
#include <string>
namespace cpu3026::commands {
	//loads bin file into memory
	inline std::vector<address_t> breakpoints;

	static bool is_breakpoint(address_t addr) {
		return std::find(breakpoints.begin(), breakpoints.end(), addr) != breakpoints.end();
	}
	int run_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
		std::string mode = "continue";
		if (argc >= 2) {
			mode = argv[1];
		}

		if(mode == "continue") {
			size_t steps = 0;
			const size_t max_steps = 1000000;
			while(steps < max_steps) {
				address_t ip = static_cast<address_t>(vm.processor()->ip());
				if (is_breakpoint(ip)) {
					cout << "Hit breakpoint at address 0x" << std::hex << ip << std::dec << "\n";
					return 0;
				}
				vm.step_once();
				steps++;
			}
			cout << "Reached maximum step count of " << max_steps << ", stopping execution.\n";
			return 0;
		}
		try {
			unsigned long iterations = std::stoul(mode, nullptr, 0);
			if (iterations == 0) {
				cout << "Number of iterations must be greater than 0.\n";
				return 2;
			}
			for(unsigned long i = 0; i < iterations; i++) {
				address_t ip = static_cast<address_t>(vm.processor()->ip());
				if (is_breakpoint(ip)) {
					cout << "Hit breakpoint at address 0x" << std::hex << ip << std::dec << "\n";
					return 0;
				}
				vm.step_once();
			}
			cout << "Executed " << iterations << " steps.\n";
		}
		catch(...) {
			cout << "Usage: run [count|continue]\n";
			return 1;
		}
	}
}


#endif // CPU3026_CLI_COMMANDS_RUN_CMD_H
