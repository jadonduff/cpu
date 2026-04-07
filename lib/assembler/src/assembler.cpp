#include <cpu3026/assembler/assembler.h>

#include <sstream>
#include <algorithm>
#include <cctype>

using namespace cpu3026;

// opcodes from ISA doc
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
static constexpr byte_t OP_PUSH    = 0x40;
static constexpr byte_t OP_POP     = 0x41;
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

// splits a line into mnemonic and operands, strips comments
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

int assembler::instruction_size(const std::string& mnemonic)
{
	// todo: confirm sizes with processor team
	if (mnemonic == "NOP"  || mnemonic == "RET" || mnemonic == "HALT") return 1;
	if (mnemonic == "NOT"  || mnemonic == "NEG" ||
	    mnemonic == "PUSH" || mnemonic == "POP")  return 2;
	if (mnemonic == "MOV"  || mnemonic == "ADD"  || mnemonic == "SUB"  ||
	    mnemonic == "MULT" || mnemonic == "DIV"  || mnemonic == "AND"  ||
	    mnemonic == "OR"   || mnemonic == "XOR"  || mnemonic == "SHL"  ||
	    mnemonic == "SHR"  || mnemonic == "CMP")  return 3;
	if (mnemonic == "LOAD"    || mnemonic == "LDBYTE" || mnemonic == "STORE" ||
	    mnemonic == "STRBYTE" || mnemonic == "LDNUM"  ||
	    mnemonic == "JMP"     || mnemonic == "CALL"   ||
	    mnemonic == "BEQ"     || mnemonic == "BNEQ"   ||
	    mnemonic == "BLT"     || mnemonic == "BGT")   return 4;
	return 0;
}

// pass 1: walk lines, record labels, count bytes
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

// resolve label or immediate
bool assembler::encode(const std::string& mnemonic, const std::string& operands,
                       address_t current_addr, std::vector<byte_t>& out, std::string& error)
{
	// split on comma
	std::vector<std::string> ops;
	std::istringstream ss(operands);
	std::string tok;
	while (std::getline(ss, tok, ','))
		ops.push_back(trim(tok));

	// helpers
	auto resolve = [&](const std::string& s, word_t& val) -> bool {
		if (parse_imm(s, val)) return true;
		auto it = symbol_table.find(s);
		if (it != symbol_table.end()) { val = it->second; return true; }
		error = "undefined '" + s + "'";
		return false;
	};

	auto emit_addr = [&](word_t a) {
		out.push_back(static_cast<byte_t>(a & 0xFF));
		out.push_back(static_cast<byte_t>((a >> 8) & 0xFF));
	};

	// no-operand
	if (mnemonic == "NOP")  { out.push_back(OP_NOP);  return true; }
	if (mnemonic == "HALT") { out.push_back(OP_HALT); return true; }
	if (mnemonic == "RET")  { out.push_back(OP_RET);  return true; }

	// single register
	if (mnemonic == "PUSH" || mnemonic == "POP" ||
	    mnemonic == "NOT"  || mnemonic == "NEG") {
		if (ops.empty()) { error = mnemonic + " needs a register"; return false; }
		int r = parse_reg(ops[0]);
		if (r < 0) { error = "bad register '" + ops[0] + "'"; return false; }

		byte_t op = 0;
		if (mnemonic == "PUSH") op = OP_PUSH;
		else if (mnemonic == "POP") op = OP_POP;
		else if (mnemonic == "NOT") op = OP_NOT;
		else op = OP_NEG;

		out.push_back(op);
		out.push_back(static_cast<byte_t>(r));
		return true;
	}

	// two register: mov, arithmetic, logic, cmp
	if (mnemonic == "MOV"  || mnemonic == "ADD"  || mnemonic == "SUB"  ||
	    mnemonic == "MULT" || mnemonic == "DIV"  || mnemonic == "AND"  ||
	    mnemonic == "OR"   || mnemonic == "XOR"  || mnemonic == "SHL"  ||
	    mnemonic == "SHR"  || mnemonic == "CMP") {
		if (ops.size() < 2) { error = mnemonic + " needs two registers"; return false; }
		int r1 = parse_reg(ops[0]);
		int r2 = parse_reg(ops[1]);
		if (r1 < 0) { error = "bad register '" + ops[0] + "'"; return false; }
		if (r2 < 0) { error = "bad register '" + ops[1] + "'"; return false; }

		byte_t op = 0;
		if (mnemonic == "MOV")       op = OP_MOV;
		else if (mnemonic == "ADD")  op = OP_ADD;
		else if (mnemonic == "SUB")  op = OP_SUB;
		else if (mnemonic == "MULT") op = OP_MULT;
		else if (mnemonic == "DIV")  op = OP_DIV;
		else if (mnemonic == "AND")  op = OP_AND;
		else if (mnemonic == "OR")   op = OP_OR;
		else if (mnemonic == "XOR")  op = OP_XOR;
		else if (mnemonic == "SHL")  op = OP_SHL;
		else if (mnemonic == "SHR")  op = OP_SHR;
		else op = OP_CMP;

		out.push_back(op);
		out.push_back(static_cast<byte_t>(r1));
		out.push_back(static_cast<byte_t>(r2));
		return true;
	}

	// register + address: load, store, ldnum
	if (mnemonic == "LOAD" || mnemonic == "LDBYTE" || mnemonic == "STORE" ||
	    mnemonic == "STRBYTE" || mnemonic == "LDNUM") {
		if (ops.size() < 2) { error = mnemonic + " needs register and address"; return false; }
		int r = parse_reg(ops[0]);
		if (r < 0) { error = "bad register '" + ops[0] + "'"; return false; }
		word_t addr = 0;
		if (!resolve(ops[1], addr)) return false;

		byte_t op = 0;
		if (mnemonic == "LOAD")        op = OP_LOAD;
		else if (mnemonic == "LDBYTE") op = OP_LDBYTE;
		else if (mnemonic == "STORE")  op = OP_STORE;
		else if (mnemonic == "STRBYTE") op = OP_STRBYTE;
		else op = OP_LDNUM;

		out.push_back(op);
		out.push_back(static_cast<byte_t>(r));
		emit_addr(addr);
		return true;
	}

	// jump/branch: address only
	// todo: confirm whether branches need a register operand too
	if (mnemonic == "JMP"  || mnemonic == "CALL" ||
	    mnemonic == "BEQ"  || mnemonic == "BNEQ" ||
	    mnemonic == "BLT"  || mnemonic == "BGT") {
		if (ops.empty()) { error = mnemonic + " needs an address"; return false; }
		word_t addr = 0;
		if (!resolve(ops[0], addr)) return false;

		byte_t op = 0;
		if (mnemonic == "JMP")       op = OP_JMP;
		else if (mnemonic == "CALL") op = OP_CALL;
		else if (mnemonic == "BEQ")  op = OP_BEQ;
		else if (mnemonic == "BNEQ") op = OP_BNEQ;
		else if (mnemonic == "BLT")  op = OP_BLT;
		else op = OP_BGT;

		out.push_back(op);
		out.push_back(0x00); // placeholder register byte
		emit_addr(addr);
		return true;
	}

	error = "unknown instruction '" + mnemonic + "'";
	return false;
}

// pass 2: encode each line
bool assembler::pass2(const std::vector<std::string>& lines, std::vector<byte_t>& out, std::string& error)
{
	address_t addr = 0;
	for (std::size_t i = 0; i < lines.size(); ++i) {
		std::string mnemonic, operands;
		if (!split_line(lines[i], mnemonic, operands)) continue;

		std::size_t before = out.size();
		if (!encode(mnemonic, operands, addr, out, error)) {
			error = "line " + std::to_string(i + 1) + ": " + error;
			return false;
		}
		addr = static_cast<address_t>(addr + (out.size() - before));
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
