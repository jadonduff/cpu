#pragma once
#ifndef CPU3026_ASSEMBLER_ASSEMBLER_H
#define CPU3026_ASSEMBLER_ASSEMBLER_H

#include <cpu3026/core/types.h>
#include <string>
#include <vector>
#include <unordered_map>

// two-pass assembler for cpu3026 asm source
// pass 1: scan labels, build symbol table
// pass 2: encode instructions to binary

namespace cpu3026 {

	struct assemble_result {
		bool                ok;
		std::vector<byte_t> binary;
		std::string         error;
	};

	class assembler {
		std::unordered_map<std::string, address_t> symbol_table;

		bool pass1(const std::vector<std::string>& lines, address_t& out_size, std::string& error);
		bool pass2(const std::vector<std::string>& lines, std::vector<byte_t>& out, std::string& error);
		bool encode(const std::string& mnemonic, const std::string& operands,
		            address_t current_addr, std::vector<byte_t>& out, std::string& error);

		static int instruction_size(const std::string& mnemonic);
		static int parse_reg(const std::string& token);
		static bool parse_imm(const std::string& token, word_t& out);

	public:
		/// @brief assembles asm source text into a binary image
		/// @param source newline-separated assembly
		/// @return result with binary bytes or error message
		assemble_result assemble(const std::string& source);
	};
}

#endif // CPU3026_ASSEMBLER_ASSEMBLER_H
