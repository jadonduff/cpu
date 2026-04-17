#pragma once
#ifndef CPU3026_CLI_COMMANDS_BREAK_CMD_H
#define CPU3026_CLI_COMMANDS_BREAK_CMD_H

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "run_cmd.h"

namespace cpu3026::commands {

	inline std::vector<address_t> breakpoints;

    static bool is_breakpoint(address_t addr) {
        return std::find(breakpoints.begin(), breakpoints.end(), addr) != breakpoints.end();
    }

	int break_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
		if (argc < 2) {
			cout << "Usage: break <addr>|list|clear <addr>" << std::endl;
			return 1;
		}

		std::string cmd = argv[1];

		if (cmd == "list") { //list breakpoints
			if (breakpoints.empty()) {
				cout << "No breakpoints set." << std::endl;
				return 0;
			}
			for (auto address : breakpoints) {
				cout << "0x" << std::hex << address << std::dec << std::endl;
			}
			return 0;
		}

		if (cmd == "clear") { //clear breakpoint at argv[2]
			if (argc < 3) {
				cout << "Usage: break clear <addr>" << std::endl;
				return 1;
			}
			address_t addr;
			try {
                addr = static_cast<address_t>(std::stoul(argv[2], nullptr, 0));
            } catch (...) {
                cout << "Invalid address: " << argv[2] << std::endl;
                return 2;
            }
            auto it = std::find(breakpoints.begin(), breakpoints.end(), addr);
            if (it == breakpoints.end()) {
                cout << "No breakpoint set at address 0x" << std::hex << addr << std::dec << std::endl;
                return 3;
            }
            breakpoints.erase(it, breakpoints.end());
            cout << "Cleared breakpoint at address 0x" << std::hex << addr << std::dec << std::endl;
            return 0;
		}

		// otherwise set a breakpoint at argv[1]
		address_t addr = 0;
		try {
            addr = static_cast<address_t>(std::stoul(cmd, nullptr, 0));
        } catch (...) {
            cout << "Invalid address: " << cmd << std::endl;
            return 2;
        
        }
        if(!is_breakpoint(addr)) {
            breakpoints.push_back(addr);
        }
            cout << "Breakpoint set at 0x" << std::hex << addr << std::dec << std::endl;

        return 0;
    }
}

#endif // CPU3026_CLI_COMMANDS_BREAK_CMD_H
