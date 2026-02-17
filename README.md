# CPU3026 Project

The CPU3026 is an emulated custom 16 bit cpu made for the class, Introduction to Software Engineering.

### Table of Contents

- [Features](#features)
- [Installation](#installation)
	- [Configuring](#configuring)
	- [Testing](#testing)
	- [Python Bindings](#python-bindings)
- [Usage](#usage)
	- [Command Line Interface](#command-line-interface)
- [Contributing](#contributing)
- [License](#license)
- [Credits](#credits)

## Features



## Installation

Make sure you have cmake version 3.16 or higher and a c++23 comipiler ready. 

### Configuring

To configure CPU3026, run

	cmake -B build

### Building Command Line Interface

To build the cmd interface, run

	cmake --build build --target cpu3026_cmd
	

### Testing

To test CPU3026 after building, run

	cmake --build build --target cpu3026_test
	cd build
	ctest -V

### Python Bindings

To build the python bindings, run

	cmake --build build --target cpu3026_py

## Usage

To run CPU3026 command line interface, download the release build and run "cpu3026_cmd.exe" (or similar name for your OS).

### Command Line Interface

## Contributing

Please read our [contributing guidelines](https://github.com/stamparkour/CPU3026/blob/main/.github/contributing.md) to get started.
For any major issues, open an issue first to discuss what should be changed.

## License

This project is under the [Apache 2.0](https://github.com/stamparkour/CPU3026/blob/main/LICENSE) license.

## Credits

