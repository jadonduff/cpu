#include <cpu3026/processor/cpu3026.h>
#include <stdexcept>

using namespace cpu3026;

// implement instructions in the // instructions header

// add instructions to cpu in function cpu3026_processor::do_step()
enum flags : word_t {
	FLAG_Z = 1 << 0, // zero flag
	FLAG_C = 1 << 1, // carry flag
	FLAG_N = 1 << 2, // negative flag
	FLAG_V = 1 << 3, // overflow flag
	FLAG_IE = 1 << 4, // interrupt enable flag
	FLAG_IP = 1 << 5, // interrupt in progress flag
};
constexpr word_t INTERRUPT_VECTOR_BASE = 0x1000;
constexpr int MAX_INTERRUPTS = 256;
// instructions
void load(cpu3026_processor& proc) { //Loads a standard word-sized value from memory into a register.
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;

	word_t address = proc.get_reg(reg_s);
	word_t value = proc.memory()->readw(address);

	proc.set_reg(reg_d, value);

	proc.ip() += 3;
}
void ldnum(cpu3026_processor& proc) {  //Loads a specific immediate numerical value directly into a register
    byte_t parameter = proc.memory()->readb(proc.ip() + 1);
	
	int reg_d = parameter & 0x07;

	word_t value = proc.memory()->readw(proc.ip() + 2);

	proc.set_reg(reg_d, value);

	proc.ip() += 4;
}
void ldbyte(cpu3026_processor& proc) { //Loads a single byte from memory into a register (useful for character processing). Clears the high byte

	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;

	word_t address = proc.get_reg(reg_s);
	byte_t value = proc.memory()->readb(address);

	proc.set_reg(reg_d, value);

	proc.ip() += 3;
}
void store(cpu3026_processor& proc) { //stores a word-sized value from a register into memory
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_s = parameter & 0x07;
	int reg_d = (parameter & 0x1C) >> 3;

	word_t address = proc.get_reg(reg_d);
	

	proc.memory()->writew(address, proc.get_reg(reg_s));

	proc.ip() += 3;
}
void strbyte(cpu3026_processor& proc) {//Stores a single byte from a register into memory.

	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_s = parameter & 0x07;
	int reg_d = (parameter & 0x1C) >> 3;

	word_t address = proc.get_reg(reg_d);

	proc.memory()->writeb(address, proc.get_reg(reg_s) & 0xFF);
	proc.ip() += 3;
}
void mov(cpu3026_processor& proc) { //Moves data from one register to another.

	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;

	proc.set_reg(reg_d, proc.get_reg(reg_s));

	proc.ip() += 3;
}
void add(cpu3026_processor& proc) { //Adds the values of two registers and stores the result in a destination register.
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;
	

	proc.set_reg(reg_d, proc.get_reg(reg_d) + proc.get_reg(reg_s));

	proc.ip() += 3;
}
void sub(cpu3026_processor& proc) { //Subtracts the value of one register from another and stores the result in a destination register.

	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;

	proc.set_reg(reg_d, proc.get_reg(reg_d) - proc.get_reg(reg_s));

	proc.ip() += 3;
}
void mult(cpu3026_processor& proc) { //Multiplies the values of two registers and stores the result in a destination register.
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;

	proc.set_reg(reg_d, proc.get_reg(reg_d) * proc.get_reg(reg_s));

	proc.ip() += 3;
}
void div(cpu3026_processor& proc) { //Divides the value of one register by another and stores the result in a destination register. Handle division by zero appropriately (e.g., set a flag or throw an exception).
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;

	word_t divisor = proc.get_reg(reg_s);
	if (divisor == 0) 
	 throw std::runtime_error("division by zero");

	proc.set_reg(reg_d, proc.get_reg(reg_d) / divisor);
	proc.ip() += 3;
}
void _and(cpu3026_processor& proc) { //perfoms a bitwise AND operation
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;

	proc.set_reg(reg_d, proc.get_reg(reg_d) & proc.get_reg(reg_s));

	proc.ip() += 3;
}
void _or(cpu3026_processor& proc) { //perfoms a bitwise OR operation
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;

	proc.set_reg(reg_d, proc.get_reg(reg_d) | proc.get_reg(reg_s));

	proc.ip() += 3;
}
void _not(cpu3026_processor& proc) {//perfoms a bitwise NOT operation
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;

	proc.set_reg(reg_d, ~proc.get_reg(reg_d));

	proc.ip() += 2;
}
void neg(cpu3026_processor& proc) { //negates the value of a register (two's complement)
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;

	proc.set_reg(reg_d, -proc.get_reg(reg_d));

	proc.ip() += 2;
}
void _xor(cpu3026_processor& proc) { //perfoms a bitwise XOR operation
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;

	proc.set_reg(reg_d, proc.get_reg(reg_d) ^ proc.get_reg(reg_s));

	proc.ip() += 3;
}
void shl(cpu3026_processor& proc) { //shifts bits to the left, identical to multiplying by 2
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;

	proc.set_reg(reg_d, proc.get_reg(reg_d) << proc.get_reg(reg_s));

	proc.ip() += 3;
}
void shr(cpu3026_processor& proc) { //shifts bits to the right, identical to dividing by 2
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_d = parameter & 0x07;
	int reg_s = (parameter & 0x1C) >> 3;

	proc.set_reg(reg_d, proc.get_reg(reg_d) >> proc.get_reg(reg_s));

	proc.ip() += 3;
}
void cmp(cpu3026_processor& proc) {//Compares the values of two registers and sets the appropriate flags (e.g., zero flag if they are equal, negative flag if the first is less than the second, etc.) without storing the result.
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_a = parameter & 0x07;
	int reg_b = (parameter & 0x1C) >> 3;

	word_t value_a = proc.get_reg(reg_a);
	word_t value_b = proc.get_reg(reg_b);

	word_t result = value_a - value_b;

	proc.set_flag(FLAG_Z, result == 0);
	proc.set_flag(FLAG_N, result < 0);
	proc.set_flag(FLAG_C, value_a < value_b);

	proc.ip() += 3;
}
void jmp(cpu3026_processor& proc) { //Unconditionally jumps to a specified memory address, updating the instruction pointer accordingly.
	word_t address = proc.memory()->readw(proc.ip() + 1);
	proc.ip() = address;
}
void call(cpu3026_processor& proc) { //calls a subroutine at a specified memory address
	word_t address = proc.memory()->readw(proc.ip() + 1);
	proc.sp() -= 2;
	proc.memory()->writew(proc.sp(), proc.ip() + 3);
	proc.ip() = address;
}
void ret(cpu3026_processor& proc) { //Returns from a subroutine to a saved address
	word_t address = proc.memory()->readw(proc.sp());
	proc.sp() += 2;

	proc.ip() = address;
}
void beq(cpu3026_processor& proc) { //jumps to a specified memory address if the zero flag is set (indicating that the previous comparison resulted in equality).
	word_t address = proc.memory()->readw(proc.ip() + 1);

	if(proc.get_flag(proc,FLAG_Z))	{
		proc.ip() = address;
	} else {
		proc.ip() += 3;
	}
}
void bneq(cpu3026_processor& proc) { //jumps to a specified memory address if the zero flag is not set (indicating that the previous comparison did not result in equality).
	word_t address = proc.memory()->readw(proc.ip() + 1);

	if(!proc.get_flag(FLAG_Z))	{
		proc.ip() = address;
	} else {
		proc.ip() += 3;
	}
}
void blt(cpu3026_processor& proc) { //jumps to a specified memory address if the negative flag is set (indicating that the first value in the previous comparison was less than the second).
	word_t address = proc.memory()->readw(proc.ip() + 1);

	if(proc.get_flag(proc,FLAG_N))	{
		proc.ip() = address;
	} else {
		proc.ip() += 3;
	}
}
void bgt(cpu3026_processor& proc) { //jumps to a specified memory address if the negative flag is not set and the zero flag is not set (indicating that the first value in the previous comparison was greater than the second).
	word_t address = proc.memory()->readw(proc.ip() + 1);

	if(!proc.get_flag(FLAG_Z) && !proc.get_flag(FLAG_N))	{
		proc.ip() = address;
	} else {
		proc.ip() += 3;
	}
}
void ble(cpu3026_processor& proc) { //jumps to a specified memory address if the zero flag is set or the negative flag is set (indicating that the first value in the previous comparison was less than or equal to the second).
	word_t address = proc.memory()->readw(proc.ip() + 1);

	if(proc.get_flag(FLAG_Z) || proc.get_flag(FLAG_N))	{
		proc.ip() = address;
	} else {
		proc.ip() += 3;
	}
}
void bge(cpu3026_processor& proc) {//jumps to a specified memory address if the negative flag is not set (indicating that the first value in the previous comparison was greater than or equal to the second).
	word_t address = proc.memory()->readw(proc.ip() + 1);

	if(!proc.get_flag(FLAG_N))	{
		proc.ip() = address;
	} else {
		proc.ip() += 3;
	}
}
void bc(cpu3026_processor& proc) {//jumps to a specified memory address if the carry flag is set (indicating that an arithmetic operation resulted in a carry out of the most significant bit).
	word_t address = proc.memory()->readw(proc.ip() + 1);

	if(proc.get_flag(FLAG_C))	{
		proc.ip() = address;
	} else {
		proc.ip() += 3;
	}
}
void bnc(cpu3026_processor& proc) {//jumps to a specified memory address if the carry flag is not set (indicating that an arithmetic operation did not result in a carry out of the most significant bit).
	word_t address = proc.memory()->readw(proc.ip() + 1);

	if(!proc.get_flag(FLAG_C))	{
		proc.ip() = address;
	} else {
		proc.ip() += 3;
	}
}
void push(cpu3026_processor& proc) {//Pushes a value from a register onto the stack, updating the stack pointer accordingly.
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg = parameter & 0x07;
	proc.sp() -= 2;
	proc.memory()->writew(proc.sp(), proc.get_reg(reg));
	proc.ip() += 2;
}
void pop(cpu3026_processor& proc) { //Pops a value from the stack into a register, updating the stack pointer accordingly.
	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg = parameter & 0x07;
	word_t value = proc.memory()->readw(proc.sp());
	proc.sp() += 2;
	proc.set_reg(reg, value);
	proc.ip() += 2;
}

void iret(cpu3026_processor& proc) {//Returns from an interrupt service routine to a saved address and restores previous flags
	word_t ip_value = proc.memory()->readw(proc.sp());
	proc.sp() += 2;

	word_t flags = proc.memory()->readw(proc.sp());
	proc.sp() += 2;

	proc.ip() = ip_value;
	proc.set_ptr(cpu_ptr::FLAGS, flags);
}
void cli(cpu3026_processor& proc) { // Clears the interrupt enable fla
	proc.set_flag(FLAG_IE, false);
	proc.ip() += 1;
}
void sti(cpu3026_processor& proc) { // Sets the interrupt enable flag
	proc.set_flag(FLAG_IE, true);
	proc.ip() += 1;
}
void intr(cpu3026_processor& proc) { // Triggers a software interrupt
	byte_t interrupt = proc.memory()->readb(proc.ip() + 1);
	proc.request_interrupt(interrupt);
}
void nop(cpu3026_processor& proc) { //no operation, does nothing and moves to the next instruction
	++proc.ip();
}
void halt(cpu3026_processor& proc) { //halts the processor, stopping execution until a reset or interrupt occurs. 
	throw std::runtime_error("cpu halted");
}
// cpu implementation

void cpu3026_processor::do_step() {
	byte_t instruction = memory()->readb(ip());
	const byte_t opcode_mask = 0b11111111;
	switch (instruction & opcode_mask) {
	case(0x00):
		load(*this);
		break;
	case(0x01):
	    ldnum(*this);
	    break;
	case(0x02):
		ldbyte(*this);
	    break;
	case(0x03):
		store(*this);
	    break;
	case(0x04):
		strbyte(*this);
		break;
	case(0x05):
	    mov(*this);
		break;
	case(0x10):
		add(*this);
		break;
	case(0x11):
		sub(*this);
		break;
	case(0x12):
		mult(*this);
		break;
	case(0x13):
		div(*this);
		break;
	case(0x20):
		_and(*this);
		break;
	case(0x21):
		_or(*this);
		break;
	case(0x22):
		_not(*this);
		break;
	case(0x23):
		neg(*this);
		break;
	case(0x24):
		_xor(*this);
		break;
	case(0x25):
		shl(*this);
		break;
	case(0x26):
		shr(*this);
		break;
	case(0x30):
		cmp(*this);
		break;
	case(0x31):
		jmp(*this);
		break;
	case(0x32):
		call(*this);
		break;
	case(0x33):
		ret(*this);
		break;
	case(0x34):
		beq(*this);
		break;
	case(0x35):
		bneq(*this);
		break;
	case(0x36):
		blt(*this);
		break;
	case(0x37):
		bgt(*this);
		break;
	case(0x38):
		ble(*this);
		break;
	case(0x39):
		bge(*this);
		break;
	case(0x3A):
		 bc(*this);
		break;
	case(0x3B):
		 bnc(*this);
		break;
	case(0x40):
		 push(*this);
		break;
	case(0x41):
		 pop(*this);
	    break;
	case(0x42):
		 iret(*this);
		break;
	case(0x43):
		 cli(*this);
		break;
	case(0x44):
		 sti(*this);
		break;
	case(0x45):
		 intr(*this);
		break;
	case(0xF0):
		nop(*this);
		break;	
	case(0xF1):
		halt(*this);
		break;
	default:
		throw std::runtime_error("invalid instruction");
	}
}
void cpu3026_processor::do_reset() {
	set_ptr(cpu_ptr::FLAGS, 0);
	set_flag(FLAG_IE, true);
	ip() = memory() > readw(INTERRUPT_VECTOR_BASE + 0);
}
void cpu3026_processor::do_interrupt(word_t interrupt) { //TODO: Implement interrupt logic
	// interrupt logic here
	set_flag(FLAG_IP, true);
	set_flag(FLAG_IE, false);
	sp() -= 2;
	memory()->writew(sp(), get_ptr(cpu_ptr::FLAGS));
	sp() -= 2;
	memory()->writew(sp(), ip());

	word_t handler_address = memory()->readw(INTERRUPT_VECTOR_BASE + interrupt * 2);
	ip() = handler_address;
	}
void cpu3026_processor::request_interrupt(word_t interrupt) {
	if(interrupt >= MAX_INTERRUPTS) throw std::runtime_error("invalid interrupt number");
	if(!get_flag(FLAG_IP) && get_flag(FLAG_IE) ) {
		do_interrupt(interrupt);
	}
}
int cpu3026_processor::do_total_cycles() {
	return total_cycles_v;
}
int cpu3026_processor::do_total_instructions() {
	return total_instructions_v;
}

cpu3026_processor::cpu3026_processor(const std::shared_ptr<memory_base>& memory, const std::shared_ptr<memory_base>& io) : memory_v(memory), io_v(io) {
	if (memory == nullptr) throw std::runtime_error("memory cannot be null");
	if (io == nullptr) throw std::runtime_error("io cannot be null");
	reset();
}

word_t cpu3026_processor::get_reg(cpu_reg_t index) const {
	if (index == 0) return 0;
	if (index > 7) throw std::runtime_error("failed to write cpu register. valid range is [0,8)");
	return reg_v[index - 1];
}
void cpu3026_processor::set_reg(cpu_reg_t index, word_t value) {
	if (index == 0 || index > 7) throw std::runtime_error("failed to write cpu register. valid range is [1,8)");
	reg_v[index - 1] = value;
}
word_t cpu3026_processor::get_ptr(cpu_ptr_t index) const {
	if (index == 0) return 0;
	if (index > 3) throw std::runtime_error("failed to read cpu pointer. valid range is [0,4)");
	return ptr_v[index - 1];
}
void cpu3026_processor::set_ptr(cpu_ptr_t index, word_t value) {
	if (index == 0 || index > 3) throw std::runtime_error("failed to write cpu pointer. valid range is [1,4)");
	ptr_v[index - 1] = value;
}
void cpu3026_processor::set_flag(word_t flag, bool value) {
	word_t flags_value = get_ptr(cpu_ptr::FLAGS);
	if (value) {
		flags_value |= flag;
	} else {
		flags_value &= ~flag;
	}
	set_ptr(cpu_ptr::FLAGS, flags_value);
}
bool cpu3026_processor::get_flag(word_t flag) const {
	word_t flags_value = get_ptr(cpu_ptr::FLAGS);
	return (flags_value & flag) != 0;
}
std::shared_ptr<memory_base> cpu3026_processor::memory() const {
	return memory_v;
}
std::shared_ptr<memory_base> cpu3026_processor::io() const {
	return io_v;
}
std::shared_ptr<memory_base>& cpu3026_processor::memory() {
	return memory_v;
}
std::shared_ptr<memory_base>& cpu3026_processor::io() {
	return io_v;
}
word_t cpu3026_processor::p0() const {
	return 0;
}
word_t& cpu3026_processor::ip() {
	return ptr_v[cpu_ptr::IP - 1];
}
word_t& cpu3026_processor::sp() {
	return ptr_v[cpu_ptr::SP - 1];
}
word_t& cpu3026_processor::bp() {
	return ptr_v[cpu_ptr::BP - 1];
}
word_t cpu3026_processor::r0() const {
	return 0;
}
word_t cpu3026_processor::ip() const {
	return ptr_v[cpu_ptr::IP - 1];
}
word_t cpu3026_processor::sp() const {
	return ptr_v[cpu_ptr::SP - 1];
}
word_t cpu3026_processor::bp() const {
	return ptr_v[cpu_ptr::BP - 1];
}
word_t& cpu3026_processor::r1() {
	return reg_v[cpu_reg::R1 - 1];
}
word_t& cpu3026_processor::r2() {
	return reg_v[cpu_reg::R2 - 1];
}
word_t& cpu3026_processor::r3() {
	return reg_v[cpu_reg::R3 - 1];
}
word_t& cpu3026_processor::r4() {
	return reg_v[cpu_reg::R4 - 1];
}
word_t& cpu3026_processor::r5() {
	return reg_v[cpu_reg::R5 - 1];
}
word_t& cpu3026_processor::r6() {
	return reg_v[cpu_reg::R6 - 1];
}
word_t& cpu3026_processor::r7() {
	return reg_v[cpu_reg::R7 - 1];
}
word_t cpu3026_processor::r1() const {
	return reg_v[cpu_reg::R1 - 1];
}
word_t cpu3026_processor::r2() const {
	return reg_v[cpu_reg::R2 - 1];
}
word_t cpu3026_processor::r3() const {
	return reg_v[cpu_reg::R3 - 1];
}
word_t cpu3026_processor::r4() const {
	return reg_v[cpu_reg::R4 - 1];
}
word_t cpu3026_processor::r5() const {
	return reg_v[cpu_reg::R5 - 1];
}
word_t cpu3026_processor::r6() const {
	return reg_v[cpu_reg::R6 - 1];
}
word_t cpu3026_processor::r7() const {
	return reg_v[cpu_reg::R7 - 1];
}
bool cpu3026_processor::flag_z() const {
	return get_flag(FLAG_Z);
}
bool cpu3026_processor::flag_n() const {
	return get_flag(FLAG_N);
}

bool cpu3026_processor::flag_v() const {
	return get_flag(FLAG_V);
}
bool cpu3026_processor::flag_c() const {
	return get_flag(FLAG_C);
}
