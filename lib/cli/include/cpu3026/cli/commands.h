#pragma once
#ifndef CPU3026_CLI_COMMANDS_H
#define CPU3026_CLI_COMMANDS_H

#include "command_handler.h"

namespace cpu3026 {
	command_handler default_commands() {
		command_handler o{};

		//o.emplace();

		return o;
	}
}

#endif // CPU3026_CLI_COMMANDS_H