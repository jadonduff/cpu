#ifndef CPU3026_MEMORY_TESTS_H
#define CPU3026_MEMORY_TESTS_H

#include <gtest/gtest.h>
#include <cpu3026/memory/memory.h>
#include <cpu3026/memory/stack.h>

using namespace cpu3026;
// test cases via google test gtest.h

TEST(Memory, RamReadWrite) {
	memory mem;
	mem.writeb(0x0000, 0xAB);
	EXPECT_EQ(mem.readb(0x0000), 0xAB);
	mem.writeb(0x3FFF, 0xCD);
	EXPECT_EQ(mem.readb(0x3FFF), 0xCD);
	mem.writeb(0x7FFF, 0xEF);
	EXPECT_EQ(mem.readb(0x7FFF), 0xEF);
}

TEST(Memory, RomReadOnly) {
	memory mem;
	std::vector<byte_t> image(ROM_SIZE, 0x55);
	mem.load_rom(image);
	EXPECT_EQ(mem.readb(ROM_BASE), 0x55);
	mem.writeb(ROM_BASE, 0xFF);
	EXPECT_EQ(mem.readb(ROM_BASE), 0x55);
}

TEST(Memory, RomHiReadOnly) {
	memory mem;
	std::vector<byte_t> image(ROM_HI_SIZE, 0x33);
	mem.load_rom_hi(image);
	EXPECT_EQ(mem.readb(ROM_HI_BASE), 0x33);
	mem.writeb(ROM_HI_BASE, 0xFF);
	EXPECT_EQ(mem.readb(ROM_HI_BASE), 0x33);
	EXPECT_EQ(mem.readb(0xFFFF), 0x33);
	mem.writeb(0xFFFF, 0xFF);
	EXPECT_EQ(mem.readb(0xFFFF), 0x33);
}

TEST(Memory, ProgromLocked) {
	memory mem;
	mem.writeb(PROGROM_BASE, 0xBB);
	EXPECT_EQ(mem.readb(PROGROM_BASE), 0x00);
}

TEST(Memory, ProgromUnlock) {
	memory mem;
	mem.writeb(PROGROM_UNLOCK_ADDR, PROGROM_UNLOCK_KEY);
	for (int i = 0; i < PROGROM_WRITE_LIMIT; i++)
		mem.writeb(static_cast<address_t>(PROGROM_BASE + i), static_cast<byte_t>(i));
	for (int i = 0; i < PROGROM_WRITE_LIMIT; i++)
		EXPECT_EQ(mem.readb(static_cast<address_t>(PROGROM_BASE + i)), static_cast<byte_t>(i));
	// next write should be ignored, progrom re-locked
	mem.writeb(PROGROM_BASE, 0xFF);
	EXPECT_EQ(mem.readb(PROGROM_BASE), 0x00);
}

TEST(Memory, UartStatus) {
	memory mem;
	// rx empty, bit0 clear, bit1 always set
	EXPECT_EQ(mem.readb(0xD001) & 0x01u, 0x00u);
	EXPECT_NE(mem.readb(0xD001) & 0x02u, 0x00u);
	mem.uart_inject_rx(0x42);
	EXPECT_NE(mem.readb(0xD001) & 0x01u, 0x00u);
	EXPECT_EQ(mem.readb(0xD000), 0x42u);
	EXPECT_EQ(mem.readb(0xD001) & 0x01u, 0x00u);
}

TEST(Memory, StackPushPopByte) {
	memory mem;
	address_t sp = static_cast<address_t>(STACK_TOP + 1); // 0x8000, empty stack
	stack_pushb(mem, sp, 0xAB);
	EXPECT_EQ(sp, STACK_TOP);
	EXPECT_EQ(mem.readb(sp), 0xABu);
	EXPECT_EQ(stack_popb(mem, sp), 0xABu);
	EXPECT_EQ(sp, static_cast<address_t>(STACK_TOP + 1));
}

TEST(Memory, StackPushPopWord) {
	memory mem;
	address_t sp = static_cast<address_t>(STACK_TOP + 1); // 0x8000, empty stack
	stack_pushw(mem, sp, 0x1234);
	EXPECT_EQ(sp, static_cast<address_t>(STACK_TOP - 1)); // moved down by 2
	EXPECT_EQ(mem.readb(sp),                              0x34u); // low byte at sp
	EXPECT_EQ(mem.readb(static_cast<address_t>(sp + 1)), 0x12u); // high byte above
	EXPECT_EQ(stack_popw(mem, sp), 0x1234u);
	EXPECT_EQ(sp, static_cast<address_t>(STACK_TOP + 1));
}

TEST(Memory, ReadWriteWord) {
	memory mem;
	mem.writew(0x0100, 0xBEEF);
	EXPECT_EQ(mem.readb(0x0100), 0xEFu); // low byte at lower address
	EXPECT_EQ(mem.readb(0x0101), 0xBEu); // high byte at higher address
	EXPECT_EQ(mem.readw(0x0100), 0xBEEFu);
}

TEST(Memory, TickTimer) {
	memory mem;
	mem.tick(0x0100);
	EXPECT_EQ(mem.readb(0xD020), 0x00u); // timer_low
	EXPECT_EQ(mem.readb(0xD021), 0x01u); // timer_high
}

#endif // CPU3026_MEMORY_TESTS_H
