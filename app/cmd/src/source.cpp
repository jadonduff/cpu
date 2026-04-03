#include <iostream>

#include <cpu3026/cli/io.h>

int main(int argc, char** argv) {
	std::cout << "CPU3026 CLI - V1" << std::endl;

	cpu3026::memory mem{};
	cpu3026::cpu3026_processor proc{ std::shared_ptr<cpu3026::memory>(&mem, [](auto p) {}), nullptr };
	cpu3026::virtual_machine vm(&mem, &proc);
	cpu3026::project_io io_device(&std::cin, &std::cout);

	while (io_device.cmd_in(vm));

	return 0;
}
