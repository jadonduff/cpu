#pragma once
#ifndef CPU3026_CLI_COMMANDS_MEM_CMD_H
#define CPU3026_CLI_COMMANDS_MEM_CMD_H

#include <iostream>
#include <iomanip>
#include <cpu3026/vm/vm.h>
#include <cpu3026/cli/command_handler.h>

namespace cpu3026::commands {

    // dump <address> [len]  e.g., "dump 0x1000 16"
    int dump_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
        if (argc < 2) {
            cout << "Usage: dump <address> [length]" << std::endl;
            return 1;
        }

        address_t addr = 0;
        int len = 16;
        try {
            std::string addr_str = argv[1];
            if (addr_str.size() > 2 && addr_str.substr(0, 2) == "0x") {
                addr = static_cast<address_t>(std::stoul(addr_str.substr(2), nullptr, 16));
            } else {
                addr = static_cast<address_t>(std::stoul(addr_str, nullptr, 10));
            }

            if (argc >= 3) {
                std::string len_str = argv[2];
                len = std::stoi(len_str);
                if (len <= 0 || len > 256) {
                    cout << "Length must be 1-256" << std::endl;
                    return 2;
                }
            }
        } catch (...) {
            cout << "Error: invalid address or length" << std::endl;
            return 3;
        }

        cout << "Dumping " << len << " bytes from 0x" << std::hex << addr << std::dec << ":" << std::endl;

        for (int i = 0; i < len; ++i) {
            if (i % 16 == 0) {
                cout << std::hex << std::setw(4) << std::setfill('0') << addr + i << ":"
                     << std::dec << " ";
            }
            cout << std::hex << std::setw(2) << std::setfill('0')
                 << static_cast<int>(vm.memory()->readb(static_cast<address_t>(addr + i)))
                 << std::dec << " ";
            if ((i + 1) % 16 == 0) cout << std::endl;
        }
        if (len % 16 != 0) cout << std::endl;

        return 0;
    }

    // setb <address> <value>  e.g., "setb 0x2000 0xAB"
    int setb_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
        if (argc != 3) {
            cout << "Usage: setb <address> <value>" << std::endl;
            return 1;
        }

        address_t addr = 0;
        byte_t value = 0;
        try {
            std::string addr_str = argv[1];
            if (addr_str.size() > 2 && addr_str.substr(0, 2) == "0x") {
                addr = static_cast<address_t>(std::stoul(addr_str.substr(2), nullptr, 16));
            } else {
                addr = static_cast<address_t>(std::stoul(addr_str, nullptr, 10));
            }

            std::string val_str = argv[2];
            if (val_str.size() > 2 && val_str.substr(0, 2) == "0x") {
                value = static_cast<byte_t>(std::stoul(val_str.substr(2), nullptr, 16));
            } else {
                value = static_cast<byte_t>(std::stoul(val_str, nullptr, 10));
            }
        } catch (...) {
            cout << "Error: invalid address or value" << std::endl;
            return 2;
        }

        vm.memory()->writeb(addr, value);
        cout << "Set memory[0x" << std::hex << addr << "] = 0x" 
             << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(value) 
             << std::dec << std::endl;
        return 0;
    }

    // getb <address>  e.g., "getb 0x2000"
    int getb_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
        if (argc != 2) {
            cout << "Usage: getb <address>" << std::endl;
            return 1;
        }

        address_t addr = 0;
        try {
            std::string addr_str = argv[1];
            if (addr_str.size() > 2 && addr_str.substr(0, 2) == "0x") {
                addr = static_cast<address_t>(std::stoul(addr_str.substr(2), nullptr, 16));
            } else {
                addr = static_cast<address_t>(std::stoul(addr_str, nullptr, 10));
            }
        } catch (...) {
            cout << "Error: invalid address" << std::endl;
            return 2;
        }

        byte_t value = vm.memory()->readb(addr);
        cout << "memory[0x" << std::hex << addr << "] = 0x" 
             << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(value) 
             << std::dec << std::endl;
        return 0;
    }

} // namespace cpu3026::commands
#endif // CPU3026_CLI_COMMANDS_MEM_CMD_H
