#include <cpu3026/processor/cpu3026.h>
#include <stdexcept>

using namespace cpu3026;

// implement instructions in the // instructions header

// add instructions to cpu in function cpu3026_processor::do_step()

// instructions

void nop(cpu3026_processor& proc) {
	++proc.ip();
}
void mov(cpu3026_processor& proc) {

	byte_t parameter = proc.memory()->readb(proc.ip() + 1);

	int reg_in = parameter & 0x07;
	int reg_out = (parameter & 0x1C) >> 3;

	proc.set_reg(reg_in, proc.get_reg(reg_out);

	proc.ip() += 3;
}

// cpu implementation

void cpu3026_processor::do_step() {
	byte_t instruction = memory()->readb(ip());
	const byte_t opcode_mask = 0b11111111;
	switch (instruction & opcode_mask) {
	case(0x00):
		mov(*this);
		break;
	case(0xF0):
		nop(*this);
		break;
	default:
		nop(*this);
		break;
	}
}
void cpu3026_processor::do_reset() {
	ip() = RESET_VECTOR;
}
void cpu3026_processor::do_interrupt(word_t interrupt) {
	// interrupt logic here
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