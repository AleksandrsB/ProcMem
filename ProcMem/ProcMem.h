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
 * @author AleksandrsB
 */
class ProcMem
{
private:
	std::wstring	m_ProcessName = L"";			/// Name of the process to be accessed.
	DWORD			m_ProcessID = 0;				/// ID of the process.
	HANDLE			m_ProcessHandle = nullptr;	/// Handle to the process.
	bool			m_isConnected = false;		/// Indicates whether the process is successfully connected.

public:
	/**
	 * @brief Constructor that initializes ProcMem with the process name and opens a handle to the process.
	 * @param procName The name of the process to connect to.
	 * @throws std::runtime_error If the toolhelp snapshot or process handle could not be created.
	 */
	ProcMem(const std::wstring& procName);

	/**
	 * @brief Destructor that closes the process handle if it is open.
	 */
	~ProcMem();

	/**
	 * @brief Retrieves the base address of a module in the process.
	 * @param moduleName The name of the module whose base address is to be retrieved.
	 * @return The base address of the module, or 0 if the module was not found.
	 * @throws std::runtime_error If creating the toolhelp snapshot fails.
	 */
	std::uintptr_t getModuleBaseAddress(const std::wstring& moduleName);

	/**
	 * @brief Retrieves the ProcessName.
	 * @return The name of the connected process.
	 */
	const std::wstring& getProcessName() const { return this->m_ProcessName; }

	/**
	 * @brief Retrieves the ProcessID (PID).
	 * @return The ProcessID of the connected process.
	 */
	DWORD getProcessID() const { return this->m_ProcessID; }

	/**
	 * @brief Retrieves the ProcessHandle.
	 * @return The ProcessHandle of the connected process.
	 */
	HANDLE getProcessHandle() const { return this->m_ProcessHandle; }

	
	/**
	 * @brief Checks if the process is connected.
	 * @return True if connected, otherwise false.
	 */
	bool getConnectedState() const { return this->m_isConnected; }

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
};
