#include <cpu3026/assembler/assembler.h>

#include <sstream>
#include <algorithm>
#include <cctype>

using namespace cpu3026;

// opcodes and param-byte layout must match cpu3026_processor::do_step().
// two-register instructions pack both registers into one param byte:
// bits 0..2 hold the register the processor decodes as reg_d (or reg_s for STORE),
// bits 3..5 hold the other. see pack_two_regs below.
static constexpr byte_t OP_LOAD    = 0x00;
static constexpr byte_t OP_LDNUM   = 0x01;
static constexpr byte_t OP_LDBYTE  = 0x02;
static constexpr byte_t OP_STORE   = 0x03;
static constexpr byte_t OP_STRBYTE = 0x04;
static constexpr byte_t OP_MOV     = 0x05;
static constexpr byte_t OP_ADD     = 0x10;
static constexpr byte_t OP_SUB     = 0x11;
static constexpr byte_t OP_MULT    = 0x12;
static constexpr byte_t OP_DIV     = 0x13;
static constexpr byte_t OP_AND     = 0x20;
static constexpr byte_t OP_OR      = 0x21;
static constexpr byte_t OP_NOT     = 0x22;
static constexpr byte_t OP_NEG     = 0x23;
static constexpr byte_t OP_XOR     = 0x24;
static constexpr byte_t OP_SHL     = 0x25;
static constexpr byte_t OP_SHR     = 0x26;
static constexpr byte_t OP_CMP     = 0x30;
static constexpr byte_t OP_JMP     = 0x31;
static constexpr byte_t OP_CALL    = 0x32;
static constexpr byte_t OP_RET     = 0x33;
static constexpr byte_t OP_BEQ     = 0x34;
static constexpr byte_t OP_BNEQ    = 0x35;
static constexpr byte_t OP_BLT     = 0x36;
static constexpr byte_t OP_BGT     = 0x37;
static constexpr byte_t OP_BLE     = 0x38;
static constexpr byte_t OP_BGE     = 0x39;
static constexpr byte_t OP_BC      = 0x3A;
static constexpr byte_t OP_BNC     = 0x3B;
static constexpr byte_t OP_PUSH    = 0x40;
static constexpr byte_t OP_POP     = 0x41;
static constexpr byte_t OP_IRET    = 0x42;
static constexpr byte_t OP_CLI     = 0x43;
static constexpr byte_t OP_STI     = 0x44;
static constexpr byte_t OP_INTR    = 0x45;
static constexpr byte_t OP_NOP     = 0xF0;
static constexpr byte_t OP_HALT    = 0xF1;

static std::string trim(const std::string& s)
{
	auto a = s.find_first_not_of(" \t\r");
	if (a == std::string::npos) return {};
	auto b = s.find_last_not_of(" \t\r");
	return s.substr(a, b - a + 1);
}

static std::string to_upper(std::string s)
{
	for (auto& c : s) c = static_cast<char>(std::toupper(c));
	return s;
}

static bool split_line(const std::string& raw, std::string& mnemonic, std::string& operands)
{
	std::string line = trim(raw);

	auto comment = line.find(';');
	if (comment != std::string::npos)
		line = trim(line.substr(0, comment));

	if (line.empty()) return false;
	if (line.back() == ':') return false; // label only

	auto sep = line.find_first_of(" \t");
	if (sep == std::string::npos) {
		mnemonic = to_upper(line);
		operands = {};
	} else {
		mnemonic = to_upper(trim(line.substr(0, sep)));
		operands = trim(line.substr(sep + 1));
	}
	return true;
}

int assembler::parse_reg(const std::string& token)
{
	std::string t = to_upper(trim(token));
	if (t.size() == 2 && t[0] == 'R' && t[1] >= '0' && t[1] <= '7')
		return t[1] - '0';
	return -1;
}

bool assembler::parse_imm(const std::string& token, word_t& out)
{
	std::string t = trim(token);
	if (t.empty()) return false;

	// hex
	if (t.size() > 2 && t[0] == '0' && (t[1] == 'x' || t[1] == 'X')) {
		unsigned long v = 0;
		for (std::size_t i = 2; i < t.size(); ++i) {
			char c = t[i];
			if (c >= '0' && c <= '9') v = v * 16 + (c - '0');
			else if (c >= 'a' && c <= 'f') v = v * 16 + (c - 'a' + 10);
			else if (c >= 'A' && c <= 'F') v = v * 16 + (c - 'A' + 10);
			else return false;
		}
		out = static_cast<word_t>(v);
		return true;
	}

	// decimal
	unsigned long v = 0;
	for (std::size_t i = 0; i < t.size(); ++i) {
		if (t[i] < '0' || t[i] > '9') return false;
		v = v * 10 + (t[i] - '0');
	}
	out = static_cast<word_t>(v);
	return true;
}

// sizes match cpu3026_processor::do_step() ip advances
int assembler::instruction_size(const std::string& mnemonic)
{
	if (mnemonic == "NOP"  || mnemonic == "HALT" ||
	    mnemonic == "RET"  || mnemonic == "IRET" ||
	    mnemonic == "CLI"  || mnemonic == "STI")   return 1;

	if (mnemonic == "NOT"  || mnemonic == "NEG"  ||
	    mnemonic == "PUSH" || mnemonic == "POP"  ||
	    mnemonic == "INTR")                          return 2;

	if (mnemonic == "JMP"  || mnemonic == "CALL" ||
	    mnemonic == "BEQ"  || mnemonic == "BNEQ" ||
	    mnemonic == "BLT"  || mnemonic == "BGT"  ||
	    mnemonic == "BLE"  || mnemonic == "BGE"  ||
	    mnemonic == "BC"   || mnemonic == "BNC")    return 3;

	if (mnemonic == "MOV"  || mnemonic == "ADD"  || mnemonic == "SUB"  ||
	    mnemonic == "MULT" || mnemonic == "DIV"  || mnemonic == "AND"  ||
	    mnemonic == "OR"   || mnemonic == "XOR"  || mnemonic == "SHL"  ||
	    mnemonic == "SHR"  || mnemonic == "CMP"  ||
	    mnemonic == "LOAD" || mnemonic == "LDBYTE" ||
	    mnemonic == "STORE" || mnemonic == "STRBYTE") return 3;

	if (mnemonic == "LDNUM") return 4;

	return 0;
}

bool assembler::pass1(const std::vector<std::string>& lines, address_t& out_size, std::string& error)
{
	address_t addr = 0;
	for (std::size_t i = 0; i < lines.size(); ++i) {
		std::string line = trim(lines[i]);

		auto comment = line.find(';');
		if (comment != std::string::npos)
			line = trim(line.substr(0, comment));

		if (line.empty()) continue;

		// label
		if (line.back() == ':') {
			std::string label = trim(line.substr(0, line.size() - 1));
			if (label.empty()) {
				error = "line " + std::to_string(i + 1) + ": empty label";
				return false;
			}
			if (symbol_table.count(label)) {
				error = "line " + std::to_string(i + 1) + ": duplicate label '" + label + "'";
				return false;
			}
			symbol_table[label] = addr;
			continue;
		}

		std::string mnemonic, operands;
		if (!split_line(line, mnemonic, operands)) continue;

		int sz = instruction_size(mnemonic);
		if (sz == 0) {
			error = "line " + std::to_string(i + 1) + ": unknown instruction '" + mnemonic + "'";
			return false;
		}
		addr = static_cast<address_t>(addr + sz);
	}
	out_size = addr;
	return true;
}

// packs two 3-bit register indices into one param byte.
// low_bits goes into bits 0..2, high_bits goes into bits 3..5.
// callers choose which register is "low" based on how the processor decodes
// that specific opcode (e.g. LOAD puts Rd low, STORE puts Rs low).
static byte_t pack_two_regs(int low_bits, int high_bits)
{
	return static_cast<byte_t>((low_bits & 0x07) | ((high_bits & 0x07) << 3));
}

bool assembler::encode(const std::string& mnemonic, const std::string& operands,
                       std::vector<byte_t>& out, std::string& error)
{
	// split operands on comma
	std::vector<std::string> ops;
	std::istringstream ss(operands);
	std::string tok;
	while (std::getline(ss, tok, ','))
		ops.push_back(trim(tok));

	auto resolve = [&](const std::string& s, word_t& val) -> bool {
		if (parse_imm(s, val)) return true;
		auto it = symbol_table.find(s);
		if (it != symbol_table.end()) { val = it->second; return true; }
		error = "undefined '" + s + "'";
		return false;
	};

	auto emit_word = [&](word_t a) {
		out.push_back(static_cast<byte_t>(a & 0xFF));
		out.push_back(static_cast<byte_t>((a >> 8) & 0xFF));
	};

	if (mnemonic == "NOP")  { out.push_back(OP_NOP);  return true; }
	if (mnemonic == "HALT") { out.push_back(OP_HALT); return true; }
	if (mnemonic == "RET")  { out.push_back(OP_RET);  return true; }
	if (mnemonic == "IRET") { out.push_back(OP_IRET); return true; }
	if (mnemonic == "CLI")  { out.push_back(OP_CLI);  return true; }
	if (mnemonic == "STI")  { out.push_back(OP_STI);  return true; }

	if (mnemonic == "PUSH" || mnemonic == "POP" ||
	    mnemonic == "NOT"  || mnemonic == "NEG") {
		if (ops.empty()) { error = mnemonic + " needs a register"; return false; }
		int r = parse_reg(ops[0]);
		if (r < 0) { error = "bad register '" + ops[0] + "'"; return false; }

		byte_t op = 0;
		if      (mnemonic == "PUSH") op = OP_PUSH;
		else if (mnemonic == "POP")  op = OP_POP;
		else if (mnemonic == "NOT")  op = OP_NOT;
		else                         op = OP_NEG;

		out.push_back(op);
		out.push_back(static_cast<byte_t>(r & 0x07));
		return true;
	}

	if (mnemonic == "INTR") {
		if (ops.empty()) { error = "INTR needs an interrupt number"; return false; }
		word_t n = 0;
		if (!resolve(ops[0], n)) return false;
		if (n > 0xFF) { error = "INTR number out of range"; return false; }
		out.push_back(OP_INTR);
		out.push_back(static_cast<byte_t>(n & 0xFF));
		return true;
	}

	if (mnemonic == "MOV"  || mnemonic == "ADD"  || mnemonic == "SUB"  ||
	    mnemonic == "MULT" || mnemonic == "DIV"  || mnemonic == "AND"  ||
	    mnemonic == "OR"   || mnemonic == "XOR"  || mnemonic == "SHL"  ||
	    mnemonic == "SHR"  || mnemonic == "CMP") {
		if (ops.size() < 2) { error = mnemonic + " needs two registers"; return false; }
		int rd = parse_reg(ops[0]);
		int rs = parse_reg(ops[1]);
		if (rd < 0) { error = "bad register '" + ops[0] + "'"; return false; }
		if (rs < 0) { error = "bad register '" + ops[1] + "'"; return false; }

		byte_t op = 0;
		if      (mnemonic == "MOV")  op = OP_MOV;
		else if (mnemonic == "ADD")  op = OP_ADD;
		else if (mnemonic == "SUB")  op = OP_SUB;
		else if (mnemonic == "MULT") op = OP_MULT;
		else if (mnemonic == "DIV")  op = OP_DIV;
		else if (mnemonic == "AND")  op = OP_AND;
		else if (mnemonic == "OR")   op = OP_OR;
		else if (mnemonic == "XOR")  op = OP_XOR;
		else if (mnemonic == "SHL")  op = OP_SHL;
		else if (mnemonic == "SHR")  op = OP_SHR;
		else                         op = OP_CMP;

		out.push_back(op);
		out.push_back(pack_two_regs(rd, rs));
		out.push_back(0x00);
		return true;
	}

	// LOAD Rd, Rs : Rd = mem[Rs]
	if (mnemonic == "LOAD" || mnemonic == "LDBYTE") {
		if (ops.size() < 2) { error = mnemonic + " needs destination and source registers"; return false; }
		int rd = parse_reg(ops[0]);
		int rs = parse_reg(ops[1]);
		if (rd < 0) { error = "bad register '" + ops[0] + "'"; return false; }
		if (rs < 0) { error = "bad register '" + ops[1] + "'"; return false; }

		byte_t op = (mnemonic == "LOAD") ? OP_LOAD : OP_LDBYTE;
		out.push_back(op);
		out.push_back(pack_two_regs(rd, rs));
		out.push_back(0x00);
		return true;
	}

	// STORE Rs, Rd : mem[Rd] = Rs.
	// processor reads reg_s from low bits and reg_d from high bits, so the
	// source register goes first in the packed byte (opposite of LOAD).
	if (mnemonic == "STORE" || mnemonic == "STRBYTE") {
		if (ops.size() < 2) { error = mnemonic + " needs source and destination registers"; return false; }
		int rs = parse_reg(ops[0]);
		int rd = parse_reg(ops[1]);
		if (rs < 0) { error = "bad register '" + ops[0] + "'"; return false; }
		if (rd < 0) { error = "bad register '" + ops[1] + "'"; return false; }

		byte_t op = (mnemonic == "STORE") ? OP_STORE : OP_STRBYTE;
		out.push_back(op);
		out.push_back(pack_two_regs(rs, rd));
		out.push_back(0x00);
		return true;
	}

	if (mnemonic == "LDNUM") {
		if (ops.size() < 2) { error = "LDNUM needs a register and an immediate"; return false; }
		int rd = parse_reg(ops[0]);
		if (rd < 0) { error = "bad register '" + ops[0] + "'"; return false; }
		word_t imm = 0;
		if (!resolve(ops[1], imm)) return false;

		out.push_back(OP_LDNUM);
		out.push_back(static_cast<byte_t>(rd & 0x07));
		emit_word(imm);
		return true;
	}

	if (mnemonic == "JMP"  || mnemonic == "CALL" ||
	    mnemonic == "BEQ"  || mnemonic == "BNEQ" ||
	    mnemonic == "BLT"  || mnemonic == "BGT"  ||
	    mnemonic == "BLE"  || mnemonic == "BGE"  ||
	    mnemonic == "BC"   || mnemonic == "BNC") {
		if (ops.empty()) { error = mnemonic + " needs an address"; return false; }
		word_t addr = 0;
		if (!resolve(ops[0], addr)) return false;

		byte_t op = 0;
		if      (mnemonic == "JMP")  op = OP_JMP;
		else if (mnemonic == "CALL") op = OP_CALL;
		else if (mnemonic == "BEQ")  op = OP_BEQ;
		else if (mnemonic == "BNEQ") op = OP_BNEQ;
		else if (mnemonic == "BLT")  op = OP_BLT;
		else if (mnemonic == "BGT")  op = OP_BGT;
		else if (mnemonic == "BLE")  op = OP_BLE;
		else if (mnemonic == "BGE")  op = OP_BGE;
		else if (mnemonic == "BC")   op = OP_BC;
		else                         op = OP_BNC;

		out.push_back(op);
		emit_word(addr);
		return true;
	}

	error = "unknown instruction '" + mnemonic + "'";
	return false;
}

bool assembler::pass2(const std::vector<std::string>& lines, std::vector<byte_t>& out, std::string& error)
{
	for (std::size_t i = 0; i < lines.size(); ++i) {
		std::string mnemonic, operands;
		if (!split_line(lines[i], mnemonic, operands)) continue;

		if (!encode(mnemonic, operands, out, error)) {
			error = "line " + std::to_string(i + 1) + ": " + error;
			return false;
		}
	}
	return true;
}

assemble_result assembler::assemble(const std::string& source)
{
	std::vector<std::string> lines;
	std::istringstream ss(source);
	std::string line;
	while (std::getline(ss, line))
		lines.push_back(line);

	symbol_table.clear();
	assemble_result result;

	address_t total = 0;
	if (!pass1(lines, total, result.error)) {
		result.ok = false;
		return result;
	}

	result.binary.reserve(total);
	if (!pass2(lines, result.binary, result.error)) {
		result.ok = false;
		return result;
	}

	result.ok = true;
	return result;
}
