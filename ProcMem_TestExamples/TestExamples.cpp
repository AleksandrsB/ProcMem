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
