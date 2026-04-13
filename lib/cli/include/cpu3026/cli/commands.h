#pragma once
#ifndef CPU3026_CLI_COMMANDS_H
#define CPU3026_CLI_COMMANDS_H

#include "command_handler.h"
#include "commands/load_cmd.h"
#include "commands/run_cmd.h"
#include "commands/reg_cmd.h"
#include "commands/mem_cmd.h"
#include <cpu3026/vm/vm.h>

namespace cpu3026 {

	using vm_command_handler = basic_command_handler<virtual_machine&>;

	inline vm_command_handler default_commands() {
		vm_command_handler o{};
		
		o.emplace("load", &commands::load_cmd);
		o.emplace("run", &commands::run_cmd);

		o.emplace("setreg", &commands::setreg_cmd);
		o.emplace("getreg", &commands::getreg_cmd);
		o.emplace("regs",  &commands::regs_cmd);
		o.alias("r",      "regs");

		o.emplace("dump", &commands::dump_cmd);
		o.emplace("setb", &commands::setb_cmd);
		o.emplace("getb", &commands::getb_cmd);
		o.alias("d", "dump");
		o.alias("sb", "setb");
		o.alias("gb", "getb");

		return o;
	}
}

#endif // CPU3026_CLI_COMMANDS_H