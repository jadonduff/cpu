#include <cpu3026/memory/memory.h>

#include <algorithm>
#include <cstring>

using namespace cpu3026;

memory::memory()
	: ram(RAM_SIZE, 0)
	, rom(ROM_SIZE, 0)
	, progrom(PROGROM_SIZE, 0)
	, rom_hi(ROM_HI_SIZE, 0)
	, gpio_data(0)
	, gpio_dir(0)
	, timer_counter(0)
	, progrom_unlocked(false)
	, progrom_writes_remaining(0)
{
	std::memset(raw_mmio, 0, sizeof(raw_mmio));
}

// mmio read, offset is address - MMIO_BASE
byte_t memory::mmio_readb(address_t offset) const
{
	switch (offset) {
	case 0x00: // uart_data, pop from rx buffer or return 0
		if (!uart_rx.empty()) {
			byte_t b = uart_rx.front();
			uart_rx.pop_front();
			return b;
		}
		return 0;
	case 0x01: // uart_status, bit0=rx_has_data, bit1=tx_ready always set
		return static_cast<byte_t>((uart_rx.empty() ? 0x00 : 0x01) | 0x02);
	case 0x10: // gpio_data
		return gpio_data;
	case 0x11: // gpio_dir
		return gpio_dir;
	case 0x20: // timer_low
		return static_cast<byte_t>(timer_counter & 0xFF);
	case 0x21: // timer_high
		return static_cast<byte_t>((timer_counter >> 8) & 0xFF);
	default:
		return raw_mmio[offset];
	}
}

// mmio write, offset is address - MMIO_BASE
void memory::mmio_writeb(address_t offset, byte_t value)
{
	switch (offset) {
	case 0x00: // uart_data, append to tx buffer
		uart_tx.push_back(value);
		break;
	case 0x01: // uart_status is read-only
		break;
	case 0x10: // gpio_data
		gpio_data = value;
		break;
	case 0x11: // gpio_dir
		gpio_dir = value;
		break;
	case 0x20: // timer_low
		timer_counter = static_cast<word_t>((timer_counter & 0xFF00u) | value);
		break;
	case 0x21: // timer_high
		timer_counter = static_cast<word_t>((timer_counter & 0x00FFu) | (static_cast<word_t>(value) << 8));
		break;
	case 0xFE: // progrom unlock register
		if (value == PROGROM_UNLOCK_KEY) {
			progrom_unlocked = true;
			progrom_writes_remaining = PROGROM_WRITE_LIMIT;
		}
		break;
	default:
		raw_mmio[offset] = value;
		break;
	}
}

byte_t memory::do_readb(address_t address) const
{
	if (address < ROM_BASE)
		return ram[address];
	if (address < PROGROM_BASE)
		return rom[address - ROM_BASE];
	if (address < MMIO_BASE)
		return progrom[address - PROGROM_BASE];
	if (address < ROM_HI_BASE)
		return mmio_readb(static_cast<address_t>(address - MMIO_BASE));
	return rom_hi[address - ROM_HI_BASE];
}

void memory::do_writeb(address_t address, byte_t value)
{
	if (address < ROM_BASE) {
		ram[address] = value;
		return;
	}
	if (address < PROGROM_BASE) {
		// rom, writes ignored
		return;
	}
	if (address < MMIO_BASE) {
		// progrom, only writable when unlocked
		if (progrom_unlocked && progrom_writes_remaining > 0) {
			progrom[address - PROGROM_BASE] = value;
			if (--progrom_writes_remaining == 0)
				progrom_unlocked = false;
		}
		return;
	}
	if (address < ROM_HI_BASE) {
		mmio_writeb(static_cast<address_t>(address - MMIO_BASE), value);
		return;
	}
	// rom_hi, writes ignored
}

void memory::load_rom(const std::vector<byte_t>& image)
{
	rom.assign(ROM_SIZE, 0);
	std::size_t len = std::min(image.size(), static_cast<std::size_t>(ROM_SIZE));
	std::copy(image.begin(), image.begin() + static_cast<std::ptrdiff_t>(len), rom.begin());
}

void memory::load_rom_hi(const std::vector<byte_t>& image)
{
	rom_hi.assign(ROM_HI_SIZE, 0);
	std::size_t len = std::min(image.size(), static_cast<std::size_t>(ROM_HI_SIZE));
	std::copy(image.begin(), image.begin() + static_cast<std::ptrdiff_t>(len), rom_hi.begin());
}

void memory::tick(dword_t cycles)
{
	timer_counter = static_cast<word_t>(timer_counter + cycles);
}

void memory::uart_inject_rx(byte_t value)
{
	uart_rx.push_back(value);
}
