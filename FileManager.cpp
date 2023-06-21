#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>
#include <fstream>
#include <conio.h>

// Функция для печати содержимого директории
void printDirectoryContents(const std::string& path) {
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA((path + "\\*").c_str(), &findData);  // Поиск первого файла/папки в директории
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {  // Проверка, является ли найденный элемент директорией
                if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                    std::cout << "<DIR>\t" << findData.cFileName << std::endl;  // Печать имени директории
                }
            }
            else {
                std::cout << "\t" << findData.cFileName << std::endl;  // Печать имени файла
            }
        } while (FindNextFileA(hFind, &findData) != 0);  // Поиск следующего файла/папки в директории
        FindClose(hFind);  // Закрытие дескриптора поиска
    }
}

// Функция для чтения содержимого текущего файла
void readCurrentFile(const std::string& filePath) {
    std::ifstream file(filePath);  // Открытие файла для чтения
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {  // Чтение файла построчно
            std::cout << line << std::endl;  // Печать текущей строки
        }
        file.close();  // Закрытие файла
    }
    else {
        std::cout << "Failed to open file: " << filePath << std::endl;  // Вывод сообщения об ошибке при открытии файла
    }
}

// Функция для получения родительской директории
std::string getParentDirectory(const std::string& path) {
    size_t found = path.find_last_of("\\");
    if (found != std::string::npos) {
        return path.substr(0, found);  // Возврат подстроки до последнего символа '\\' в пути
    }
    return "";
}

// Структура для хранения информации о системе
struct SystemInfo {
    std::string manufacturer;
    std::string model;
    std::string serialNumber;
    std::string biosVersion;
};

// Структура для хранения информации о процессоре
struct ProcessorInfo {
    std::string name;
    std::string architecture;
    std::string description;
    DWORD numberOfCores;
};

// Структура для хранения информации о памяти
struct MemoryInfo {
    DWORDLONG totalPhysicalMemory;
    DWORDLONG availablePhysicalMemory;
};

// Функция для получения информации о системе
SystemInfo getSystemInfo() {
    SystemInfo systemInfo;

    char buffer[256];
    DWORD bufferSize = sizeof(buffer);

    // Получение модели системы
    bufferSize = sizeof(buffer);
    if (GetComputerNameExA(ComputerNameDnsHostname, buffer, &bufferSize)) {
        systemInfo.model = buffer;
    }

    // Получение серийного номера системы
    bufferSize = sizeof(buffer);
    if (GetSystemFirmwareTable('RSMB', 0, buffer, bufferSize)) {
        systemInfo.serialNumber = buffer;
    }

    // Получение версии BIOS
    bufferSize = sizeof(buffer);
    if (RegGetValueA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", "BIOSVersion", RRF_RT_ANY, nullptr, buffer, &bufferSize) == ERROR_SUCCESS) {
        systemInfo.biosVersion = buffer;
    }

    return systemInfo;
}

// Функция для получения информации о процессоре
ProcessorInfo getProcessorInfo() {
    ProcessorInfo processorInfo;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);  // Получение информации о системе

    processorInfo.numberOfCores = sysInfo.dwNumberOfProcessors;

    switch (sysInfo.wProcessorArchitecture) {  // Определение архитектуры процессора
    case PROCESSOR_ARCHITECTURE_AMD64:
        processorInfo.architecture = "x64";
        break;
    case PROCESSOR_ARCHITECTURE_ARM:
        processorInfo.architecture = "ARM";
        break;
    case PROCESSOR_ARCHITECTURE_IA64:
        processorInfo.architecture = "Itanium-based";
        break;
    case PROCESSOR_ARCHITECTURE_INTEL:
        processorInfo.architecture = "x86";
        break;
    case PROCESSOR_ARCHITECTURE_UNKNOWN:
    default:
        processorInfo.architecture = "Unknown";
        break;
    }

    // Получение имени процессора
    char buffer[256];
    DWORD bufferSize = sizeof(buffer);
    RegGetValueA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "ProcessorNameString", RRF_RT_ANY, nullptr, buffer, &bufferSize);
    processorInfo.name = buffer;

    // Получение описания процессора
    bufferSize = sizeof(buffer);
    RegGetValueA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "Identifier", RRF_RT_ANY, nullptr, buffer, &bufferSize);
    processorInfo.description = buffer;

    return processorInfo;
}

// Функция для получения информации о памяти
MemoryInfo getMemoryInfo() {
    MemoryInfo memoryInfo;

    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatusEx(&memoryStatus);  // Получение информации о памяти

    memoryInfo.totalPhysicalMemory = memoryStatus.ullTotalPhys;
    memoryInfo.availablePhysicalMemory = memoryStatus.ullAvailPhys;

    return memoryInfo;
}

// Функция для печати информации о системе
void printSystemInfo(const SystemInfo& systemInfo) {
    std::cout << "System Information" << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << "Model: " << systemInfo.model << std::endl;
    std::cout << "Serial Number: " << systemInfo.serialNumber << std::endl;
    std::cout << "BIOS Version: " << systemInfo.biosVersion << std::endl;
    std::cout << std::endl;
}

// Функция для печати информации о процессоре
void printProcessorInfo(const ProcessorInfo& processorInfo) {
    std::cout << "Processor Information" << std::endl;
    std::cout << "---------------------" << std::endl;
    std::cout << "Name: " << processorInfo.name << std::endl;
    std::cout << "Architecture: " << processorInfo.architecture << std::endl;
    std::cout << "Description: " << processorInfo.description << std::endl;
    std::cout << "Number of Cores: " << processorInfo.numberOfCores << std::endl;
    std::cout << std::endl;
}

// Функция для печати информации о памяти
void printMemoryInfo(const MemoryInfo& memoryInfo) {
    std::cout << "Memory Information" << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << "Total Physical Memory: " << memoryInfo.totalPhysicalMemory << " bytes" << std::endl;
    std::cout << "Available Physical Memory: " << memoryInfo.availablePhysicalMemory << " bytes" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::string currentPath = ".";

    while (true) {
        system("cls");  // Очистка экрана
        SystemInfo systemInfo = getSystemInfo();
        ProcessorInfo processorInfo = getProcessorInfo();
        MemoryInfo memoryInfo = getMemoryInfo();
        printSystemInfo(systemInfo);
        printProcessorInfo(processorInfo);
        printMemoryInfo(memoryInfo);
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        currentPath = buffer;
        std::cout << "Current Directory: " << currentPath << std::endl;
        std::cout << "-----------------------------" << std::endl;
        printDirectoryContents(currentPath);  // Вывод содержимого текущей директории
        std::cout << std::endl;
        std::cout << "Enter a folder or file name to navigate or view its contents" << std::endl;
        std::cout << "Enter '..' to go to the parent directory" << std::endl;
        std::cout << "Enter 'exit' to quit" << std::endl;
        std::cout << "Input: ";

        std::string input;
        std::getline(std::cin, input);

        if (input == "exit") {  // Завершение программы при вводе "exit"
            break;
        }
        else if (input == "..") {  // Переход в родительскую директорию при вводе ".."
            currentPath = getParentDirectory(currentPath);
            SetCurrentDirectoryA(currentPath.c_str());  // Изменение текущей директории
        }
        else {
            std::string newPath = currentPath + "\\" + input;

            DWORD attributes = GetFileAttributesA(newPath.c_str());
            if (attributes != INVALID_FILE_ATTRIBUTES) {
                if (attributes & FILE_ATTRIBUTE_DIRECTORY) {  // Переход в выбранную директорию
                    currentPath = newPath;
                    SetCurrentDirectoryA(currentPath.c_str());  // Изменение текущей директории
                }
                else {
                    readCurrentFile(newPath);  // Чтение содержимого выбранного файла
                    std::cout << std::endl;
                    std::cout << "Press any key to continue..." << std::endl;
                    _getch();  // Ожидание нажатия клавиши
                }
            }
            else {
                std::cout << "Invalid path or file name" << std::endl;
                std::cout << "Press any key to continue..." << std::endl;
                _getch();  // Ожидание нажатия клавиши
            }
        }
    }

    return 0;
}


// Приведенный код является примером программы на языке C++,
// которая предоставляет интерактивную оболочку командной строки для просмотра информации о системе,
// процессоре, памяти и навигации по файловой системе в Windows.

// Программа использует WinAPI для получения информации о системе,
// процессоре и памяти, а также для выполнения операций с файловой системой,
// таких как чтение содержимого директории и чтение файлов.
