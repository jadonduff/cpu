## Overview

The memory subsystem routes every address the CPU touches to the correct backing storage or device. See memory.h and memory.cpp in lib/memory/ for the full implementation.

---

## Memory Regions

### RAM (0x0000-0x7FFF)

General-purpose read/write storage where programs and data live.

- 32KiB of space.
    
- do_readb(address) - returns the byte contained in the given memory address.
    
- do_writeb(address, byteValue) - sets the byte at the given memory address to the provided value.
    
- These two functions technically accept every address, but RAM is the only area of memory that can always be written to.
    

### ROM (0x8000-0xBFFF)

Read-only storage loaded from a binary image at startup.

- 16KiB of space.
    
- ROM cannot be written to with do_writeb(). The following function must be used to replace it.
    
- load_rom(image) - Copies the data of a given binary image into ROM. The image will be truncated or zero-added to fit the space.
    

### PROGROM (0xC000-0xCFFF)

A lockable programmable ROM region that is read-only by default.

- 4KiB of space.
    
- The write-lock is secured with both the boolean progrom_unlocked and the integer progrom_writes_remaining.
    

#### PROGROM Unlock Mechanism

The unlock is triggered by writing a key value to a specific MMIO address, allowing a limited number of writes before re-locking.

- When the byte defined in PROGROM_UNLOCK_KEY (0x5A at the time of writing) is written to a key address defined in PROGROM_UNLOCK_ADDR (0xD0FE at the time of writing), the write-lock on the PROGROM region is lifted for a limited number of writes defined in PROGROM_WRITE_LIMIT (256 at the time of writing).
    
- This is done by setting progrom_writes_remaining to PROGROM_WRITE_LIMIT and progrom_unlocked to true.
    
- With every byte written to PROGROM, progrom_writes_remaining decrements by 1. Once it returns to 0, progrom_unlocked is reset to false, and future writes will be ignored.
    

### ROM_HI (0xD100-0xFFFF)

A second read-only region typically used for interrupt vectors and boot code.

- 12032B (11.75KiB) of space.
    
- Similar to the main ROM, it cannot be edited with individual writes. A full binary image must be loaded to replace it.
    
- The image will be truncated or zero-added to fit the space.
    

---

## MMIO Devices (0xD000-0xD0FF)

Memory-mapped I/O lets the CPU talk to hardware by reading and writing to special addresses instead of real memory. See mmio_readb and mmio_writeb in memory.cpp.

This 256-byte range acts as a collection of hardware ports and status flags.

### UART (0xD000-0xD001)

A simple serial port with separate transmit and receive buffers.

- The uart_tx vector is a buffer that stores bytes to be sent to the peripheral.
    
- The double-ended queue uart_rx is a buffer of bytes received from the peripheral.
    

#### 0xD000 - UART Data

The data register for sending and receiving bytes over the serial port.

- When reading from this address, the earliest unread byte in the uart_rx buffer is popped and returned. If there is no such byte, a 0 is returned instead.
    
- When writing to this address, the input byte is appended to the end of the uart_tx buffer.
    

#### 0xD001 - UART Status

A read-only status register with flags indicating whether data is available to read and whether the transmitter is ready.

- When there is data waiting to be received from uart_rx, bit 0 is active.
    
- When the transfer buffer uart_tx is ready to send more data, bit 1 is active. (For now, this should always be true.)
    

### GPIO (0xD010-0xD011)

Two registers that control general-purpose digital I/O pins.

- See the gpio_data and gpio_dir members in memory.h.
    
- …alright, I’m a bit lost on these ones.
    

#### 0xD010 - GPIO Data

The register that holds the current value of the I/O pins.

- See the 0x10 case in mmio_readb and mmio_writeb.
    

#### 0xD011 - GPIO Direction

The register that controls whether each pin acts as input or output.

- See the 0x11 case in mmio_readb and mmio_writeb.
    

### Timer (0xD020-0xD021)

A 16-bit cycle counter that the CPU increments each tick, exposed as two byte-wide registers.

- See tick() and the timer_counter member in memory.h.
    
- The tick(numCycles) function adds a specified number of cycles to the clock.
    
- (…not much more I can think to add.)
    

#### 0xD020 - Timer Low Byte

The low 8 bits of the cycle counter.

- See the 0x20 case in mmio_readb and mmio_writeb.
    

#### 0xD021 - Timer High Byte

The high 8 bits of the cycle counter.

- See the 0x21 case in mmio_readb and mmio_writeb.
    

---

## Stack Helpers

The stack is a region of RAM used for function calls, growing downward from the top of memory.

- See stack.h in lib/memory/include/cpu3026/memory/.
    

### push/pop byte

Functions that write or read a single byte to the stack and move the stack pointer accordingly.

- See stack_pushb and stack_popb in stack.h.
    
- stack_pushb(memoryBase, stackPointer, byteValue) decrements the stack pointer in the given memory region(?), then pushes byteValue onto the stack.
    
- stack_popb(memoryBase, stackPointer) pops the top-most byte from the stack, then increments the pointer.
    

### push/pop word

Functions that push or pop a 16-bit value as two bytes, stored in little-endian order on the stack.

- stack_pushw has similar parameters to the byte variant. It stores the high-byte of the word before the low-byte. If a single byte is popped, the low-byte is retrieved first.
    
- stack_popw pops two bytes, then re-combines them in big-endian form before returning the word.
    

### Stack Frame Layout

The calling convention defines how saved registers, return addresses, and arguments are arranged on the stack during a function call.

- See frame_arg_addr in stack.h for how argument positions are computed.
    
- …I’m a bit lost here.
    

---

Key File Table

|                                            |                                                                    |
| ------------------------------------------ | ------------------------------------------------------------------ |
| File                                       | Purpose                                                            |
| lib/memory/include/cpu3026/memory/memory.h | class definition, constants, and member declarations               |
| lib/memory/src/memory.cpp                  | full implementation of address routing and MMIO                    |
| lib/memory/include/cpu3026/memory/stack.h  | inline stack push/pop helpers and frame utilities                  |
| test/src/memory_tests.h                    | google test cases covering all memory regions and stack operations |
