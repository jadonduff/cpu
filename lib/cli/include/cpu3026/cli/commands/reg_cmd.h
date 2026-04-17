#pragma once
#ifndef CPU3026_CLI_COMMANDS_REG_CMD_H
#define CPU3026_CLI_COMMANDS_REG_CMD_H

#include <iostream>
#include <sstream>
#include <cpu3026/vm/vm.h>
#include <cpu3026/cli/command_handler.h>
#include <cpu3026/processor/cpu3026.h>
#include <iomanip>
namespace cpu3026::commands {

    // Helper: Parse register name ("r0".."r7" or "ip" "sp" "bp" "p0")
    inline bool parse_reg(const std::string& name, cpu3026_processor& proc, bool& is_ptr, int& idx) {
        if (name.size() == 2 && name[0] == 'r' && name[1] >= '0' && name[1] <= '7') {
            is_ptr = false;
            idx = name[1] - '0';  // '0' → 0, '1' → 1, ..., '7' → 7
            return true;
        }
        if (name == "ip") { is_ptr = true; idx = cpu3026::cpu_ptr::IP; return true; }
        if (name == "sp") { is_ptr = true; idx = cpu3026::cpu_ptr::SP; return true; }
        if (name == "bp") { is_ptr = true; idx = cpu3026::cpu_ptr::BP; return true; }
        if (name == "p0") { is_ptr = true; idx = cpu3026::cpu_ptr::P0; return true; }
        return false;
    }

    // setreg <reg> <value>  (e.g., "setreg r1 0xDEAD" or "setreg sp 0x7000")
    int setreg_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
        if (argc != 3) {
            cout << "Usage: setreg <register> <value>" << std::endl;
            cout << "  Registers: r0-r7, ip, sp, bp, p0" << std::endl;
            cout << "  Value: hex (0x...), decimal, or binary (0b...)" << std::endl;
            return 1;
        }

        std::string reg_name = argv[1];
        std::string val_str = argv[2];

        // Parse register
        bool is_ptr;
        int idx;
        if (!parse_reg(reg_name, *vm.processor(), is_ptr, idx)) {
            cout << "Error: unknown register '" << reg_name << "'" << std::endl;
            return 2;
        }

        // Parse value (auto-detect base: 0x=hex, 0b=binary, else decimal)
        word_t value = 0;
        try {
            std::string clean = val_str;
            if (clean.size() > 2 && clean.substr(0, 2) == "0x") {
                value = static_cast<word_t>(std::stoul(clean.substr(2), nullptr, 16));
            } else if (clean.size() > 2 && clean.substr(0, 2) == "0b") {
                value = static_cast<word_t>(std::stoul(clean.substr(2), nullptr, 2));
            } else {
                value = static_cast<word_t>(std::stoul(clean, nullptr, 10));
            }
        } catch (...) {
            cout << "Error: invalid value '" << val_str << "'" << std::endl;
            return 3;
        }

        // Set register
        if (is_ptr) {
            if (idx == cpu3026::cpu_ptr::P0) {
                cout << "Note: P0 is read-only (always 0), value ignored." << std::endl;
            } else {
                vm.processor()->set_ptr(static_cast<cpu3026::cpu_ptr_t>(idx), value);
            }
        } else {
            if (idx == 0) {
                cout << "Note: R0 is read-only (always 0), value ignored." << std::endl;
            } else {
                vm.processor()->set_reg(static_cast<cpu3026::cpu_reg_t>(idx), value);
            }
        }

        cout << "Set " << reg_name << " = 0x" << std::hex << value << std::dec << std::endl;
        return 0;
    }

    // getreg <reg>  (e.g., "getreg r1")
    int getreg_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
        if (argc != 2) {
            cout << "Usage: getreg <register>" << std::endl;
            cout << "  Registers: r0-r7, ip, sp, bp, p0" << std::endl;
            return 1;
        }

        std::string reg_name = argv[1];

        // Parse register
        bool is_ptr;
        int idx;
        if (!parse_reg(reg_name, *vm.processor(), is_ptr, idx)) {
            cout << "Error: unknown register '" << reg_name << "'" << std::endl;
            return 2;
        }

        word_t value;
        if (is_ptr) {
            if (idx == cpu3026::cpu_ptr::P0) {
                value = 0;
            } else {
                value = vm.processor()->get_ptr(static_cast<cpu3026::cpu_ptr_t>(idx));
            }
        } else {
            if (idx == 0) {
                value = 0;  // r0 always 0
            } else {
                value = vm.processor()->get_reg(static_cast<cpu3026::cpu_reg_t>(idx));
            }
        }

        cout << reg_name << " = 0x" << std::hex << value << std::dec << std::endl;
        return 0;
    }

    // regs  (display all registers)
    int regs_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
        cout << "Registers:" << std::endl;

        // Show general-purpose registers
        cout << "  r0 = 0x" << std::hex << vm.processor()->r0() << std::dec << std::endl;
        cout << "  r1 = 0x" << std::hex << vm.processor()->r1() << std::dec << std::endl;
        cout << "  r2 = 0x" << std::hex << vm.processor()->r2() << std::dec << std::endl;
        cout << "  r3 = 0x" << std::hex << vm.processor()->r3() << std::dec << std::endl;
        cout << "  r4 = 0x" << std::hex << vm.processor()->r4() << std::dec << std::endl;
        cout << "  r5 = 0x" << std::hex << vm.processor()->r5() << std::dec << std::endl;
        cout << "  r6 = 0x" << std::hex << vm.processor()->r6() << std::dec << std::endl;
        cout << "  r7 = 0x" << std::hex << vm.processor()->r7() << std::dec << std::endl;

        // Show special registers (IP, SP, BP, P0)
        cout << "  p0 = 0x" << std::hex << vm.processor()->p0() << std::dec << std::endl;
        cout << "  ip = 0x" << std::hex << vm.processor()->ip() << std::dec << std::endl;
        cout << "  sp = 0x" << std::hex << vm.processor()->sp() << std::dec << std::endl;
        cout << "  bp = 0x" << std::hex << vm.processor()->bp() << std::dec << std::endl;

        // Show flags
        auto& proc = *vm.processor();
        cout << "  flags = 0x" << std::hex << proc.get_flag_reg() << std::dec 
             << " (Z=" << proc.flag_z()
             << ", N=" << proc.flag_n()
             << ", C=" << proc.flag_c()
             << ", O=" << proc.flag_o() << ", IE=" << proc.get_flag(cpu3026::FLAG_IE)
             << ", IP=" << proc.get_flag(cpu3026::FLAG_IP) << ")" << std::endl;

        return 0;
    }
    int stack_cmd(int argc, char** argv, virtual_machine& vm, std::istream& cin, std::ostream& cout) {
        int count = 16; // default stack depth
        if (argc >= 2){
            try {
                count =std::stoi(argv[1]);
                if (count <= 0 || count > 256) {
                    cout << "Count must be 1-256" << std::endl;
                    return 1;
                }
            } catch (...) {
                cout << "Error: invalid count" << std::endl;
                return 2;
            }
        }
        auto* mem = vm.memory();
        auto* proc = vm.processor();

        address_t sp = proc->sp();
        address_t bp = proc->bp();

        cout << "Stack (SP=0x" << std::hex << sp << ", BP=0x" << bp << std::dec << "):" << std::endl;

        for(int i = 0; i < count; i++) {
            address_t addr = sp + i * 2;
            if (addr >= 0x10000) break; // prevent overflow
            word_t value = mem->readw(addr);
            cout << std::hex << std::setw(4) << std::setfill('0') << addr << ": 0x" 
                 << std::setw(4) << std::setfill('0') << value << std::dec;
            if (addr == sp) cout << " <-- SP";
            if (addr == bp) cout << " <-- BP";
            cout << std::dec << std::endl;
     
        }
        return 0;
    }    
} // namespace cpu3026::commands
#endif // CPU3026_CLI_COMMANDS_REG_CMD_H
