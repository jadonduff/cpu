#pragma once
#include <cpu3026/core/types.h>

namespace cpu3026 {
	/// @brief memory interface class
	/// contains virtual functions to read and write cpu3026::byte_t and cpu3026::word_t
	class memory_base {
	protected:
		virtual byte_t do_readb(address_t address) const = 0;
		virtual void do_writeb(address_t address, byte_t value) = 0;
		virtual word_t do_readw(address_t address) const;
		virtual void do_writew(address_t address, word_t value);
	public:
		/// @brief reads cpu3026::byte_t from memory at address
		/// @param address place to read from memory
		/// @return byte at address
		byte_t readb(address_t address) const;
		/// @brief writes cpu3026::byte_t to memory at address
		/// @param address place to write in memory
		/// @param value value to write into memory
		void writeb(address_t address, byte_t value);
		/// @brief reads cpu3026::word_t from memory at address
		/// @param address place to read from memory
		/// @return word at address
		word_t readw(address_t address) const;
		/// @brief writes cpu3026::word_t to memory at address
		/// @param address place to write in memory
		/// @param value value to write into memory
		void writew(address_t address, word_t value);
	};

	//definitions

	inline word_t cpu3026::memory_base::do_readw(address_t address) const {
		return static_cast<word_t>(do_readb(address) | (do_readb(address+1) << 8));
	}
	inline void cpu3026::memory_base::do_writew(address_t address, word_t value) {
		do_writeb(address, static_cast<byte_t>(value & 0xFF));
		do_writeb(address + 1, static_cast<byte_t>((value >> 8) & 0xFF));
	}
	byte_t cpu3026::memory_base::readb(address_t address) const {
		return do_readb(address);
	}
	inline void cpu3026::memory_base::writeb(address_t address, byte_t value) {
		return do_writeb(address, value);
	}
	inline word_t cpu3026::memory_base::readw(address_t address) const {
		return do_readw(address);
	}
	inline void cpu3026::memory_base::writew(address_t address, word_t value) {
		return do_writew(address, value);
	}
}