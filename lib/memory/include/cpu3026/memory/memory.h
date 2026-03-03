#pragma once
#ifndef CPU3026_MEMORY_MEMORY_H
#define CPU3026_MEMORY_MEMORY_H

#include <cpu3026/core/memory_base.h>
#include <cpu3026/core/types.h>
#include <deque>
#include <vector>

// basic memory mapped ram and rom, ability to load .bin files into rom
// 16-bit byte-addressed, 8-bit data bus, little-endian

namespace cpu3026 {

	// address space layout constants
	static constexpr address_t RAM_SIZE = 0x8000; // 32 KiB, 0x0000-0x7FFF
	static constexpr address_t ROM_BASE = 0x8000;
	static constexpr address_t ROM_SIZE = 0x4000; // 16 KiB
	static constexpr address_t PROGROM_BASE = 0xC000;
	static constexpr address_t PROGROM_SIZE = 0x1000; // 4 KiB
	static constexpr address_t MMIO_BASE = 0xD000;
	static constexpr address_t MMIO_SIZE = 0x0100; // 256 bytes
	static constexpr address_t ROM_HI_BASE = 0xD100;
	static constexpr address_t ROM_HI_SIZE = 0x2F00; // 0xD100-0xFFFF = 12032 bytes

	// stack lives in the upper end of ram by convention
	static constexpr address_t STACK_BOTTOM = 0x7000;
	static constexpr address_t STACK_TOP = 0x7FFF;
	// initial sp = STACK_TOP + 1 = 0x8000, meaning empty stack

	// write PROGROM_UNLOCK_KEY to PROGROM_UNLOCK_ADDR to unlock
	// next PROGROM_WRITE_LIMIT writes succeed before re-locking
	static constexpr byte_t PROGROM_UNLOCK_KEY  = 0x5A;
	static constexpr address_t PROGROM_UNLOCK_ADDR = 0xD0FE;
	static constexpr int PROGROM_WRITE_LIMIT = 256;

	class memory : public memory_base {
		// backing storage for each memory region
		std::vector<byte_t> ram;
		std::vector<byte_t> rom;
		std::vector<byte_t> progrom;
		std::vector<byte_t> rom_hi;

		// mmio device state
		mutable std::deque<byte_t> uart_rx; // readb consumes from this buffer
		std::vector<byte_t> uart_tx;
		byte_t gpio_data;
		byte_t gpio_dir;
		word_t timer_counter;
		byte_t raw_mmio[MMIO_SIZE]; // fallback for unhandled mmio addresses

		// progrom write lock
		bool progrom_unlocked;
		int  progrom_writes_remaining;

		byte_t mmio_readb(address_t offset) const;
		void   mmio_writeb(address_t offset, byte_t value);

	public:
		memory();

		/// @brief loads a binary image into rom, truncated or zero-padded to ROM_SIZE
		/// @param image source bytes
		void load_rom(const std::vector<byte_t>& image);
		/// @brief loads a binary image into rom_hi, truncated or zero-padded to ROM_HI_SIZE
		/// @param image source bytes
		void load_rom_hi(const std::vector<byte_t>& image);
		/// @brief increments the 16-bit timer counter, wraps on overflow
		/// @param cycles number of elapsed cycles to add
		void tick(dword_t cycles);
		/// @brief injects a byte into the uart rx buffer for simulation or testing
		/// @param value byte to inject
		void uart_inject_rx(byte_t value);

	private:
		byte_t do_readb(address_t address) const override;
		void   do_writeb(address_t address, byte_t value) override;
	};
}

#endif // CPU3026_MEMORY_MEMORY_H
