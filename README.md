# ProcMem

ProcMem is a C++ library for reading and writing memory in a process. This library supports both 32-bit and 64-bit architectures.

## Features
- Use the process name to connect and obtain a handle.
- Read values of various types from the process memory.
- Write values of various types to the process memory.
- Retrieve the base address of a module in the process.
- Support for both single values and arrays.

## Files

- `ProcMem.h`: Contains the `ProcMem` class with all functions and their implementations.
- `example_test.cpp`: Example test file demonstrating how to use the `ProcMem` class.

## Usage

1. Include `ProcMem.h` in your project.
2. Use the `ProcMem` class to connect/attach to a process and read/write its memory.

### Example

```cpp
#include <iostream>
#include <ProcMem.h>

int main()
{
	const std::wstring procName = L"notepad.exe";

	ProcMem pm(procName);

	if (!pm.getConnectedState())
	{
		std::cout << "Failed to connect to process!" << std::endl;
		return 0;
	}

	std::cout << "Successfully connected to the process!\nProcessID: " << std::hex << pm.getProcessID() << std::endl;

	int value = pm.readMemory<int>({L"USER32.dll", 0x0});
	bool assertEqual = value == 0x00905A4D;

	value = pm.readMemory<int>({ L"USER32.dll", 0x0 });
	assertEqual = value == 0x00905A4D;

	std::cout << "Value: " << value << ", assertEqual: " << (assertEqual ? "true" : "false") << std::endl;
}
