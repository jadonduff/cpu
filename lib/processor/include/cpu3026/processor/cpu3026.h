#pragma once
#ifndef CPU3026_PROCESSOR_CPU3026_H
#define CPU3026_PROCESSOR_CPU3026_H

#include <cpu3026/core/types.h>
#include <cpu3026/core/processor_base.h>
#include <cpu3026/core/memory_base.h>
#include <memory>

namespace cpu3026 {
	using cpu_reg_t = unsigned;
	namespace cpu_reg {
		enum : cpu_reg_t {
			R0 = 0,
			R1 = 1,
			R2 = 2,
			R3 = 3,
			R4 = 4,
			R5 = 5,
			R6 = 6,
			R7 = 7,
		};
	}
	using cpu_ptr_t = unsigned;
	namespace cpu_ptr {
		enum : cpu_ptr_t {
			P0 = 0,
			IP = 1,
			SP = 2,
			BP = 3,
		};
	}

	// position in memory where the processor looks when reset.
	constexpr address_t RESET_VECTOR = 0x0000;

	class cpu3026_processor : public processor_base {
		int total_cycles_v = 0;
		int total_instructions_v = 0;

		word_t reg_v[7]{};
		word_t ptr_v[3]{};

		std::shared_ptr<memory_base> memory_v;
		std::shared_ptr<memory_base> io_v;
	public:

		cpu3026_processor(const std::shared_ptr<memory_base>& memory, const std::shared_ptr<memory_base>& io);
	private:
		void	do_reset()						override;
		void	do_step()						override;
		void	do_interrupt(word_t interrupt)	override;
		int		do_total_cycles()				override;
		int		do_total_instructions()			override;
	public:
		// array based get-set registers and pointers
		word_t	get_reg(cpu_reg_t) const;
		word_t	get_ptr(cpu_ptr_t) const;
		void	set_reg(cpu_reg_t, word_t value);
		void	set_ptr(cpu_ptr_t, word_t value);

		// pointer accessors
		word_t& ip();
		word_t& bp();
		word_t& sp();
		word_t p0() const;
		word_t ip() const;
		word_t bp() const;
		word_t sp() const;

		// register accessors
		word_t& r1();
		word_t& r2();
		word_t& r3();
		word_t& r4();
		word_t& r5();
		word_t& r6();
		word_t& r7();
		word_t r0() const;
		word_t r1() const;
		word_t r2() const;
		word_t r3() const;
		word_t r4() const;
		word_t r5() const;
		word_t r6() const;
		word_t r7() const;

		// memory accessors
		std::shared_ptr<memory_base>& memory();
		std::shared_ptr<memory_base>& io();
		std::shared_ptr<memory_base> memory() const;
		std::shared_ptr<memory_base> io() const;
	};
}

#endif // CPU3026_PROCESSOR_CPU3026_H