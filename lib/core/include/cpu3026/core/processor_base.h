#pragma once
#ifndef CPU3026_CORE_PROCESSOR_BASE_H
#define CPU3026_CORE_PROCESSOR_BASE_H

#include <cpu3026/core/types.h>

namespace cpu3026 {
	class processor_base {
	protected:
		virtual void do_reset() = 0;
		virtual void do_step() = 0;
		virtual void do_interrupt(word_t interrupt) = 0;
		virtual int do_total_cycles() = 0;
		virtual int do_total_instructions() = 0;
		
	public:
		/// @brief resets the processor to the default state
		void reset();
		/// @brief runs count steps in the processor
		/// @param count number of steps to run, default is 1
		void step(int count = 1);
		/// @brief interrupts the processor. threadsafe
		/// @param interrupt index of the interrupt to trigger
		void interrupt(word_t interrupt);

		int total_cycles();
		int total_instructions();
	};

	//definitions

	void cpu3026::processor_base::reset() {
		do_reset();
	}
	inline void cpu3026::processor_base::step(int count) {
		while(count-- > 0) do_step();
	}
	inline void cpu3026::processor_base::interrupt(word_t id) {
		do_interrupt(id);
	}
	inline int cpu3026::processor_base::total_cycles() {
		return do_total_cycles();
	}
	inline int cpu3026::processor_base::total_instructions() {
		return do_total_instructions();
	}
}

#endif // CPU3026_CORE_PROCESSOR_BASE_H