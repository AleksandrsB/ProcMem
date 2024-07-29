#include "ProcMem.h"

ProcMem::ProcMem(const std::wstring& procName)
{
	this->ProcessName = procName;

	ProcessID = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(pe);
		if (Process32First(snapshot, &pe)) {
			do {
				if (ProcessName == pe.szExeFile) {
					ProcessID = pe.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &pe));
		}
		CloseHandle(snapshot);
	}
	else throw std::runtime_error("Failed to create toolhelp snapshot");

	if (ProcessID != 0) {
		ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
		if (ProcessHandle != nullptr) {
			isConnected = true;
		}
	}
}

ProcMem::~ProcMem()
{
	if (ProcessHandle) {
		CloseHandle(ProcessHandle);
		ProcessHandle = nullptr;
	}
}

std::uintptr_t ProcMem::GetModuleBaseAddress(const std::wstring& moduleName)
{
	if (ProcessID == 0) return 0;

	std::uintptr_t baseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID);
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 me;
		me.dwSize = sizeof(me);
		if (Module32First(hSnapshot, &me)) {
			do {
				if (moduleName == me.szModule) {
					baseAddress = reinterpret_cast<std::uintptr_t>(me.modBaseAddr);
					break;
				}
			} while (Module32Next(hSnapshot, &me));
		}
		CloseHandle(hSnapshot);
	}
	else throw std::runtime_error("Failed to create toolhelp snapshot");
	return baseAddress;
}