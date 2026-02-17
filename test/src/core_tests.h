#ifndef CPU3026_CORE_TESTS_H
#define CPU3026_CORE_TESTS_H

#include <gtest/gtest.h>
#include <cpu3026/core/types.h>
#include <cpu3026/core/memory_base.h>
#include <cpu3026/core/processor_base.h>

using namespace cpu3026;

class memory_test : public memory_base {
	virtual byte_t do_readb(address_t address) const {
		return buffer[address];
	}
	virtual void do_writeb(address_t address, byte_t value) {
		buffer[address] = value;
	}
public:
	byte_t buffer[0xFFFF + 1]{};

	memory_test() {}
};

TEST(memory_base, verify_read_and_write) {
	memory_test mem{};
	memory_base* m_ptr = &mem;

	m_ptr->writew(0, 0xDEAD);
	ASSERT_EQ(m_ptr->readw(0), 0xDEAD) << "failed to write word 0xDEAD or readw and writew dont match";
	ASSERT_EQ(m_ptr->readb(0), 0xAD) << "writew is not little endian or failed to writew first byte";
	ASSERT_EQ(m_ptr->readb(1), 0xDE) << "writew is not little endian or failed to writew second byte";
	m_ptr->writeb(2, 0xAF);
	m_ptr->writeb(3, 0xBE);
	ASSERT_EQ(m_ptr->readw(2), 0xBEAF) << "readw is not little endian or failed to writeb";
}

#endif // CPU3026_CORE_TESTS_H
