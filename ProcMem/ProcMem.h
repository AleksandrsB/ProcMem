#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <stdexcept>
#include <vector>

/**
 * @class ProcMem
 * @brief A class for reading and writing memory in a process.
 *
 * @author Aleksandrs Buraks
 */
class ProcMem
{
private:
	HANDLE			ProcessHandle = nullptr;	/// Handle to the process.

public:
	DWORD			ProcessID = 0;				/// ID of the process.
	std::wstring	ProcessName = L"";			/// Name of the process to be accessed.
	bool			isConnected = false;		/// Indicates whether the process is successfully connected.

	/**
	 * @brief Constructor that initializes ProcMem with the process name and opens a handle to the process.
	 * @param procName The name of the process to connect to.
	 * @throws std::runtime_error If the toolhelp snapshot or process handle could not be created.
	 */
	ProcMem(const std::wstring& procName)
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

	/**
	 * @brief Destructor that closes the process handle if it is open.
	 */
	~ProcMem()
	{
		if (ProcessHandle) {
			CloseHandle(ProcessHandle);
			ProcessHandle = nullptr;
		}
	}

	/**
	 * @brief Reads a value of type T from the specified address in the process memory.
	 * @tparam T The type of the value to be read.
	 * @param fromAddr The address in the process memory to read from.
	 * @return The value read from the memory.
	 * @throws std::runtime_error If reading the memory fails.
	 */
	template<typename T>
	inline T readMemory(std::uintptr_t fromAddr)
	{
		T value;
		if (ReadProcessMemory(ProcessHandle, reinterpret_cast<LPCVOID>(fromAddr), &value, sizeof(T), nullptr))
			return value;
		throw std::runtime_error("Failed to read memory");
	}

	/**
	 * @brief Reads an array of values of type T from the specified address in the process memory.
	 * @tparam T The type of the values to be read.
	 * @param fromAddr The starting address in the process memory to read from.
	 * @param count The number of elements to read.
	 * @return A vector containing the values read from the memory.
	 * @throws std::runtime_error If reading the memory array fails.
	 */
	template<typename T>
	inline std::vector<T> readMemoryArray(std::uintptr_t fromAddr, size_t count)
	{
		std::vector<T> buffer(count);
		if (ReadProcessMemory(ProcessHandle, reinterpret_cast<LPCVOID>(fromAddr), buffer.data(), sizeof(T) * count, nullptr))
			return buffer;
		throw std::runtime_error("Failed to read memory array");
	}

	/**
	 * @brief Writes a value of type T to the specified address in the process memory.
	 * @tparam T The type of the value to be written.
	 * @param toAddr The address in the process memory to write to.
	 * @param value The value to write to the memory.
	 * @return True if the write operation was successful.
	 * @throws std::runtime_error If writing to the memory fails.
	 */
	template<typename T>
	inline bool writeMemory(std::uintptr_t toAddr, T value)
	{
		SIZE_T bytesWritten;
		bool success = WriteProcessMemory(ProcessHandle, reinterpret_cast<LPVOID>(toAddr), reinterpret_cast<LPCVOID>(&value), sizeof(T), &bytesWritten);

		if (!success || (bytesWritten != sizeof(T)))
			throw std::runtime_error("Failed to write memory");
		return true;
	}

	/**
	 * @brief Writes an array of values of type T to the specified address in the process memory.
	 * @tparam T The type of the values to be written.
	 * @param toAddr The starting address in the process memory to write to.
	 * @param values Pointer to the array of values to write.
	 * @param count The number of elements to write.
	 * @return True if the write operation was successful.
	 * @throws std::runtime_error If writing to the memory array fails.
	 */
	template<typename T>
	inline bool writeMemoryArray(std::uintptr_t toAddr, const T* values, size_t count)
	{
		SIZE_T bytesWritten;
		bool success = WriteProcessMemory(ProcessHandle, reinterpret_cast<LPVOID>(toAddr), values, sizeof(T) * count, &bytesWritten);

		if (!success || (bytesWritten != sizeof(T) * count))
			throw std::runtime_error("Failed to write memory array");
		return true;
	}

	/**
	 * @brief Retrieves the base address of a module in the process.
	 * @param moduleName The name of the module whose base address is to be retrieved.
	 * @return The base address of the module, or 0 if the module was not found.
	 * @throws std::runtime_error If creating the toolhelp snapshot fails.
	 */
	std::uintptr_t GetModuleBaseAddress(const std::wstring& moduleName)
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
};
