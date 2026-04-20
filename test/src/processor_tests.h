#ifndef CPU3026_PROCESSOR_TESTS_H
#define CPU3026_PROCESSOR_TESTS_H

#include <gtest/gtest.h>
#include <cpu3026/processor/cpu3026.h>
#include <memory>
#include <vector>
#include <cstring>

using namespace cpu3026;

// -----------------------------
// Simple fake memory backend
// -----------------------------
class FakeMemory : public memory_base {
public:
    std::vector<byte_t> mem;

    FakeMemory(size_t size = 65536) : mem(size, 0) {}

    byte_t readb(word_t addr) override {
        return mem[addr];
    }

    word_t readw(word_t addr) override {
        word_t val = 0;
        std::memcpy(&val, &mem[addr], sizeof(word_t));
        return val;
    }

    void writeb(word_t addr, byte_t value) override {
        mem[addr] = value;
    }

    void writew(word_t addr, word_t value) override {
        std::memcpy(&mem[addr], &value, sizeof(word_t));
    }
};

// helper to encode register byte (d in low 3 bits, s in next 3 bits)
static byte_t encode(byte_t d, byte_t s) {
    return (d & 0x07) | ((s & 0x07) << 3);
}

// -----------------------------
// CPU instruction tests
// -----------------------------

TEST(CPU3026, LoadWordFromMemory) {
    auto mem = std::make_shared<FakeMemory>();
    cpu3026_processor cpu(mem, nullptr);

    cpu.ip() = 0;

    // setup:
    // R2 = address 100
    cpu.set_reg(2, 100);

    word_t value = 42;
    mem->writew(100, value);

    // instruction: load R1 <- [R2]
    mem->writeb(0, 0x00);
    mem->writeb(1, encode(1, 2));

    cpu.do_step();

    EXPECT_EQ(cpu.get_reg(1), 42);
    EXPECT_EQ(cpu.ip(), 3);
}

TEST(CPU3026, LoadImmediateNumber) {
    auto mem = std::make_shared<FakeMemory>();
    cpu3026_processor cpu(mem, nullptr);

    cpu.ip() = 0;

    mem->writeb(0, 0x01);
    mem->writeb(1, encode(3, 0));
    mem->writew(2, 1234);

    cpu.do_step();

    EXPECT_EQ(cpu.get_reg(3), 1234);
    EXPECT_EQ(cpu.ip(), 4);
}

TEST(CPU3026, AddRegisters) {
    auto mem = std::make_shared<FakeMemory>();
    cpu3026_processor cpu(mem, nullptr);

    cpu.ip() = 0;

    cpu.set_reg(1, 10);
    cpu.set_reg(2, 5);

    mem->writeb(0, 0x10);
    mem->writeb(1, encode(1, 2));

    cpu.do_step();

    EXPECT_EQ(cpu.get_reg(1), 15);
}

TEST(CPU3026, StoreWordToMemory) {
    auto mem = std::make_shared<FakeMemory>();
    cpu3026_processor cpu(mem, nullptr);

    cpu.ip() = 0;

    cpu.set_reg(2, 77);   // value
    cpu.set_reg(1, 200);  // address

    mem->writeb(0, 0x03);
    mem->writeb(1, encode(2, 1));

    cpu.do_step();

    EXPECT_EQ(mem->readw(200), 77);
}

TEST(CPU3026, CompareSetsFlags) {
    auto mem = std::make_shared<FakeMemory>();
    cpu3026_processor cpu(mem, nullptr);

    cpu.ip() = 0;

    cpu.set_reg(1, 10);
    cpu.set_reg(2, 10);

    mem->writeb(0, 0x30);
    mem->writeb(1, encode(1, 2));

    cpu.do_step();

    EXPECT_TRUE(cpu.flag_z());
    EXPECT_FALSE(cpu.flag_n());
}

TEST(CPU3026, JumpInstruction) {
    auto mem = std::make_shared<FakeMemory>();
    cpu3026_processor cpu(mem, nullptr);

    cpu.ip() = 0;

    mem->writeb(0, 0x31);
    mem->writew(1, 500);

    cpu.do_step();

    EXPECT_EQ(cpu.ip(), 500);
}

#endif // CPU3026_PROCESSOR_TESTS_H#ifndef CPU3026_PROCESSOR_TESTS_H
#define CPU3026_PROCESSOR_TESTS_H

#include <gtest/gtest.h>

// TEST(TestSuiteName, TestName) {}

#endif // CPU3026_PROCESSOR_TESTS_H
