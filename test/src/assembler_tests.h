#ifndef CPU3026_ASSEMBLER_TESTS_H
#define CPU3026_ASSEMBLER_TESTS_H

#include <gtest/gtest.h>
#include <cpu3026/assembler/assembler.h>
#include <cpu3026/memory/memory.h>
#include <cpu3026/processor/cpu3026.h>
#include <memory>
#include <string>

using namespace cpu3026;

// end-to-end: assembler output runs correctly on the processor, 
//making a small program to test
TEST(Assembler, SmokeAddTwoImmediates) {
	const std::string src =
		"start:\n"
		"  LDNUM R1, 0x1234\n"
		"  LDNUM R2, 0x0001\n"
		"  ADD R1, R2\n"
		"  HALT\n";

	assembler asm_;
	assemble_result r = asm_.assemble(src);
	ASSERT_TRUE(r.ok) << "assembler error: " << r.error;
	EXPECT_EQ(r.binary.size(), 12u);

	auto mem = std::make_shared<memory>();
	auto io  = std::make_shared<memory>();
	for (std::size_t i = 0; i < r.binary.size(); ++i)
		mem->writeb(static_cast<address_t>(i), r.binary[i]);
	// processor reset reads ip from the interrupt vector, so point it at our program
	mem->writew(INTERRUPT_VECTOR_BASE, 0x0000);

	cpu3026_processor cpu(mem, io);
	EXPECT_EQ(cpu.ip(), 0x0000u);

	cpu.step();
	EXPECT_EQ(cpu.get_reg(cpu_reg::R1), 0x1234u);
	cpu.step();
	EXPECT_EQ(cpu.get_reg(cpu_reg::R2), 0x0001u);
	cpu.step();
	EXPECT_EQ(cpu.get_reg(cpu_reg::R1), 0x1235u);
	EXPECT_EQ(cpu.get_reg(cpu_reg::R2), 0x0001u);

	// HALT is implemented as a thrown runtime_error, so reaching it proves execution got there
	EXPECT_THROW(cpu.step(), std::runtime_error);
}

#endif // CPU3026_ASSEMBLER_TESTS_H
