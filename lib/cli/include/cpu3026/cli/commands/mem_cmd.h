#pragma once
#ifndef CPU3026_CLI_COMMANDS_MEM_CMD_H
#define CPU3026_CLI_COMMANDS_MEM_CMD_H

#include <iostream>
#include <iomanip>
#include <cpu3026/vm/vm.h>
#include <cpu3026/cli/command_handler.h>
#include <string>
#include <sstream>

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

    // disasm <address> [count]  disassemble instructions starting at address
    int disasm_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
        if (argc < 2) {
            cout << "Usage: disasm <address> [count]" << std::endl;
            return 1;
        }

        address_t addr = 0;
        int count = 16;
        try {
            std::string addr_str = argv[1];
            if (addr_str.size() > 2 && addr_str.substr(0, 2) == "0x") {
                addr = static_cast<address_t>(std::stoul(addr_str.substr(2), nullptr, 16));
            } else {
                addr = static_cast<address_t>(std::stoul(addr_str, nullptr, 10));
            }
            if (argc >= 3) {
                count = std::stoi(argv[2]);
                if (count <= 0 || count > 1024) {
                    cout << "Count must be 1-1024" << std::endl;
                    return 2;
                }
            }
        } catch (...) {
            cout << "Error: invalid address or count" << std::endl;
            return 3;
        }

        auto readb = [&](address_t a) -> byte_t { return vm.memory()->readb(a); };
        auto readw = [&](address_t a) -> word_t { return vm.memory()->readw(a); };

        address_t cur = addr;
        for (int i = 0; i < count; ++i) {
            byte_t opc = readb(cur);
            std::ostringstream out;
            out << std::hex << std::setw(4) << std::setfill('0') << cur << ": ";

            int size = 1;
            switch (opc) {
            case 0x00: { // load
                byte_t p = readb(cur + 1);
                out << "load r" << (p & 0x07) << ", [r" << ((p >> 3) & 0x07) << "]";
                size = 3;
                break;
            }
            case 0x01: { // ldnum
                byte_t p = readb(cur + 1);
                word_t imm = readw(cur + 2);
                out << "ldnum r" << (p & 0x07) << ", 0x" << std::hex << imm;
                size = 4;
                break;
            }
            case 0x02: { // ldbyte
                byte_t p = readb(cur + 1);
                out << "ldbyte r" << (p & 0x07) << ", [r" << ((p >> 3) & 0x07) << "]";
                size = 3;
                break;
            }
            case 0x03: { out << "store ..."; size = 3; break; }
            case 0x04: { out << "strbyte ..."; size = 3; break; }
            case 0x05: { out << "mov ..."; size = 3; break; }
            case 0x10: { out << "add ..."; size = 3; break; }
            case 0x11: { out << "sub ..."; size = 3; break; }
            case 0x12: { out << "mult ..."; size = 3; break; }
            case 0x13: { out << "div ..."; size = 3; break; }
            case 0x20: { out << "and ..."; size = 3; break; }
            case 0x21: { out << "or ..."; size = 3; break; }
            case 0x22: { out << "not ..."; size = 2; break; }
            case 0x23: { out << "neg ..."; size = 2; break; }
            case 0x24: { out << "xor ..."; size = 3; break; }
            case 0x25: { out << "shl ..."; size = 3; break; }
            case 0x26: { out << "shr ..."; size = 3; break; }
            case 0x30: { out << "cmp ..."; size = 3; break; }
            case 0x31: { {
                    word_t a = readw(cur + 1);
                    out << "jmp 0x" << std::hex << a;
                    size = 3;
                } break;
            }
            case 0x32: { word_t a = readw(cur + 1); out << "call 0x" << std::hex << a; size = 3; break; }
            case 0x33: { out << "ret"; size = 1; break; }
            case 0x34: { word_t a = readw(cur + 1); out << "beq 0x" << std::hex << a; size = 3; break; }
            case 0x35: { word_t a = readw(cur + 1); out << "bneq 0x" << std::hex << a; size = 3; break; }
            case 0x36: { word_t a = readw(cur + 1); out << "blt 0x" << std::hex << a; size = 3; break; }
            case 0x37: { word_t a = readw(cur + 1); out << "bgt 0x" << std::hex << a; size = 3; break; }
            case 0x38: { word_t a = readw(cur + 1); out << "ble 0x" << std::hex << a; size = 3; break; }
            case 0x39: { word_t a = readw(cur + 1); out << "bge 0x" << std::hex << a; size = 3; break; }
            case 0x3A: { word_t a = readw(cur + 1); out << "bc 0x" << std::hex << a; size = 3; break; }
            case 0x3B: { word_t a = readw(cur + 1); out << "bnc 0x" << std::hex << a; size = 3; break; }
            case 0x40: { byte_t p = readb(cur + 1); out << "push r" << (p & 0x07); size = 2; break; }
            case 0x41: { byte_t p = readb(cur + 1); out << "pop r" << (p & 0x07); size = 2; break; }
            case 0x42: { out << "iret"; size = 1; break; }
            case 0x43: { out << "cli"; size = 1; break; }
            case 0x44: { out << "sti"; size = 1; break; }
            case 0x45: { byte_t v = readb(cur + 1); out << "intr 0x" << std::hex << static_cast<int>(v); size = 2; break; }
            case 0xF0: { out << "nop"; size = 1; break; }
            case 0xF1: { out << "halt"; size = 1; break; }
            default: { out << "db 0x" << std::hex << static_cast<int>(opc); size = 1; break; }
            }

            // print raw bytes
            out << "\t\t";
            for (int b = 0; b < size; ++b) {
                out << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(readb(cur + b)) << " ";
            }

            cout << out.str() << std::endl;
            cur += size;
        }

        return 0;
    }

} // namespace cpu3026::commands
#endif // CPU3026_CLI_COMMANDS_MEM_CMD_H