#include <iostream>
#include "ProcMem.h"


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

	std::uintptr_t user32Addr = pm.getModuleBaseAddress(L"USER32.dll");
	if (user32Addr == 0)
	{
		std::cout << "USER32.dll module not found in " << procName.c_str();
		return 0;
	}

	std::cout << "USER32.dll found at: " << std::hex << user32Addr << std::endl;
	int value = pm.readMemory<int>(user32Addr);
	bool assertEqual = value == 0x00905A4D;

	std::cout << "Value: " << value << ", assertEqual: " << (assertEqual ? "true" : "false") << std::endl;
}
