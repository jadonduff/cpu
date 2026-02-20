#pragma once
#ifndef CPU3026_CORE_TYPES_H
#define CPU3026_CORE_TYPES_H
#include <cstdint>
#include <cstddef>

namespace cpu3026 {
	using byte_t = std::uint16_t;
	using word_t = std::uint16_t;
	using dword_t = std::uint32_t;
	using address_t = std::uint16_t;
}

#endif // CPU3026_CORE_TYPES_H