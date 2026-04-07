#pragma once
#ifndef CPU3026_VM_VM_H
#define CPU3026_VM_VM_H


#include <cpu3026/memory/memory.h>
#include <cpu3026/processor/cpu3026.h>
#include <cpu3026/core/types.h>
#include <cstdlib>

// ability to bind events to specifc addresses, used for io ports

namespace cpu3026 {
	class virtual_machine {
		cpu3026::memory* memory_v;
		cpu3026_processor* processor_v;
	public:
		virtual_machine(memory* memory, cpu3026_processor* processor) : memory_v(memory), processor_v(processor) { }

		cpu3026::memory* memory() {
			return memory_v;
		}
		const cpu3026::memory* memory() const {
			return memory_v;
		}
		cpu3026_processor* processor() {
			return processor_v;
		}
		const cpu3026_processor* processor() const {
			return processor_v;
		}

		void step_once() {
			processor()->step();
		}
		void step(std::size_t itterations) {
			for (int i = 0; i < itterations; i++) {
				step_once();
			}
		}

	};
}

#endif // CPU3026_VM_VM_H