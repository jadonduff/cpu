#pragma once
#ifndef CPU3026_MEMORY_IO_MEMORY_H
#define CPU3026_MEMORY_IO_MEMORY_H

#include <cpu3026/core/memory_base.h>
#include <cpu3026/core/types.h>

// ability to bind events to specifc addresses, used for io ports

namespace cpu3026 {
	class io_memory : public memory_base {
		// add mapped io here
	public:
		io_memory();

		// add functions to add io bindings
	private:
		byte_t do_readb(address_t address) const override;
		void do_writeb(address_t address, byte_t value) override;
		word_t do_readw(address_t address) const override;
		void do_writew(address_t address, word_t value) override;
	};
}

#endif // CPU3026_MEMORY_IO_MEMORY_H