# ProcMem Project

A simple library for reading and writing memory in a process.

## Files

- `ProcMem.h`: Contains the `ProcMem` class with all functions and their implementations.
- `example_test.cpp`: Example test file demonstrating how to use the `ProcMem` class.

## Usage

1. Include `ProcMem.h` in your project.
2. Use the `ProcMem` class to connect/attach to a process and read/write its memory.

### Example

```cpp
#include <iostream>
#include "ProcMem.h"

int main()
{
    const std::wstring procName = L"notepad.exe";

    ProcMem pm(procName);

    if (!pm.isConnected)
    {
        std::cout << "Failed to connect to process!" << std::endl;
        return 0;
    }

    std::cout << "Successfully connected to the process!\nProcessID: " << std::hex << pm.ProcessID << std::endl;

    std::uintptr_t user32Addr = pm.GetModuleBaseAddress(L"USER32.dll");
    if (user32Addr == 0)
    {
        std::cout << "USER32.dll module not found in " << procName.c_str();
        return 0;
    }

    std::cout << "USER32.dll found at: " << std::hex << user32Addr << std::endl;
	
	// The first 4 bytes of USER32.dll are: 0x4D 0x5A 0x90 0x00 (or 0x00905A4D in 4-byte hex)
	// Let's check it.
    int value = pm.readMemory<int>(user32Addr);
    bool assertEqual = value == 0x00905A4D;

    std::cout << "Value: " << value << ", assertEqual: " << (assertEqual ? "true" : "false") << std::endl;
}