#include "ProcMem.h"

ProcMem::ProcMem(const std::wstring& procName)
{
	this->m_ProcessName = procName;
	this->m_ProcessID = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Failed to create toolhelp snapshot");
	}

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	if (Process32First(snapshot, &pe)) {
		do {
			if (this->m_ProcessName == pe.szExeFile) {
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
				if (hProcess) {
					PROCESS_MEMORY_COUNTERS pmc;
					if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
						if (pmc.WorkingSetSize > 0) {  // Check if process is active
							this->m_ProcessID = pe.th32ProcessID;
							CloseHandle(hProcess);
							break;
						}
					}
					CloseHandle(hProcess);
				}
			}
		} while (Process32Next(snapshot, &pe));
	}
	CloseHandle(snapshot);

	if (this->m_ProcessID != 0) {
		this->m_ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->m_ProcessID);
		if (this->m_ProcessHandle != nullptr) {
			this->m_isConnected = true;
		}
	}
}

ProcMem::~ProcMem()
{
	if (this->m_ProcessHandle) {
		CloseHandle(this->m_ProcessHandle);
		this->m_ProcessHandle = nullptr;
	}
}

std::uintptr_t ProcMem::getModuleBaseAddress(const std::wstring& moduleName)
{
	if (this->m_ProcessID == 0) return 0;

	// Check if the module base address is already cached
	auto it = m_ModuleCache.find(moduleName);
	if (it != m_ModuleCache.end())
	{
		return it->second;
	}

	std::uintptr_t baseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->m_ProcessID);
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 me;
		me.dwSize = sizeof(me);
		if (Module32First(hSnapshot, &me)) {
			do {
				if (moduleName == me.szModule) {
					baseAddress = reinterpret_cast<std::uintptr_t>(me.modBaseAddr);
					// Cache and return the base address
					m_ModuleCache[moduleName] = baseAddress;
					break;
				}
			} while (Module32Next(hSnapshot, &me));
		}
		m_ModuleCache[moduleName] = 0;
		CloseHandle(hSnapshot);
	}
	else throw std::runtime_error("Failed to create toolhelp snapshot");
	return baseAddress;
}