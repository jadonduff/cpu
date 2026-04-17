#pragma once
#ifndef CPU3026_CLI_COMMANDS_INTERRUPT_CMD_H
#define CPU3026_CLI_COMMANDS_INTERRUPT_CMD_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <cpu3026/vm/vm.h>

namespace cpu3026::commands {

    int interrupt_cmd(int argc, char** argv, virtual_machine& vm, std::istream& /*cin*/, std::ostream& cout) {
        if (argc != 2) {
            cout << "Usage: interrupt <num>" << std::endl;
            return 1;
        }
        unsigned long val = 0;
        try { val = std::stoul(argv[1], nullptr, 0); } catch (...) { cout << "Invalid interrupt number" << std::endl; return 2; }
        vm.processor()->request_interrupt(static_cast<word_t>(val));
        cout << "Requested interrupt " << val << std::endl;
        return 0;
    }
}

#endif // CPU3026_CLI_COMMANDS_INTERRUPT_CMD_H
