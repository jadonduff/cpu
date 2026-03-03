#pragma once
#ifndef CPU3026_MEMORY_STACK_H
#define CPU3026_MEMORY_STACK_H

#include <cpu3026/core/memory_base.h>
#include <cpu3026/core/types.h>

/*
	stack helper functions for the cpu3026 calling convention
	stack grows downward in ram, initial sp = STACK_TOP + 1 meaning empty
*/

namespace cpu3026 {

	/// @brief pushes a byte onto the stack, sp decremented first
	inline void stack_pushb(memory_base& mem, address_t& sp, byte_t v)
	{
		mem.writeb(--sp, v);
	}

	/// @brief pops a byte from the stack, sp incremented after
	inline byte_t stack_popb(memory_base& mem, address_t& sp)
	{
		return mem.readb(sp++);
	}

	/// @brief pushes a word onto the stack, little-endian
	/// high byte pushed first so [sp] = low byte after the call
	inline void stack_pushw(memory_base& mem, address_t& sp, word_t v)
	{
		stack_pushb(mem, sp, static_cast<byte_t>(v >> 8));
		stack_pushb(mem, sp, static_cast<byte_t>(v & 0xFF));
	}

	/// @brief pops a word from the stack, little-endian
	inline word_t stack_popw(memory_base& mem, address_t& sp)
	{
		byte_t lo = stack_popb(mem, sp);
		byte_t hi = stack_popb(mem, sp);
		return static_cast<word_t>(lo | (static_cast<word_t>(hi) << 8));
	}

	/// @brief returns the address of the nth 16-bit argument relative to bp
	/// frame: [bp+0..1] saved bp, [bp+2..3] return addr, [bp+4..] args
	inline address_t frame_arg_addr(address_t bp, address_t index)
	{
		return static_cast<address_t>(bp + 4 + 2 * index);
	}
}

#endif // CPU3026_MEMORY_STACK_H
