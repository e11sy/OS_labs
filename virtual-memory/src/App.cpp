#include <windows.h>
#include <iostream>
#include <string>
#include <locale>

SIZE_T GetDefaultRegionSize() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwAllocationGranularity;
}

std::string getBinaryMask(DWORD mask) {
    std::string result;
    for (int i = sizeof(DWORD)*8 - 1; i >= 0; --i) {
        result.push_back((mask & (1 << i)) ? '1' : '0');
    }
    size_t pos = result.find_first_not_of('0');
    return (pos != std::string::npos) ? result.substr(pos) : "0";
}

void FreeAllocatedMemory(void* reservedRegion) {
    if (VirtualFree(reservedRegion, 0, MEM_RELEASE)) {
        std::cout << "Память очищена." << std::endl;
    } else {
        std::cout << "Ошибка во время очистки памяти, Код ошибки: " << GetLastError() << std::endl;
    }
}

void showSystemInfo() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    std::cout << "Информация о системе:" << std::endl;
    std::string architecture;
    switch(si.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_INTEL:
            architecture = "x86 (32-bit)";
            break;
        case PROCESSOR_ARCHITECTURE_AMD64:
            architecture = "x64 (64-bit)";
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            architecture = "ARM";
            break;
        case PROCESSOR_ARCHITECTURE_ARM64:
            architecture = "ARM64";
            break;
        default:
            architecture = "Неизвестный тип";
            break;
    }
    std::cout << "Архитектура процессора: " << architecture << std::endl;
    std::cout << "Размер страницы: " << si.dwPageSize << " b" << std::endl;
    std::cout << "Минимальный адрес приложения: " << si.lpMinimumApplicationAddress << std::endl;
    std::cout << "Максимальнай адрес приложения: " << si.lpMaximumApplicationAddress << std::endl;
    std::cout << "Маска активного процессора: " << getBinaryMask(si.dwActiveProcessorMask) << std::endl;
    std::cout << "Число процессоров: " << si.dwNumberOfProcessors << std::endl;
    std::string processorType;
    switch(si.dwProcessorType) {
        case PROCESSOR_INTEL_386:
            processorType = "Intel 386";
            break;
        case PROCESSOR_INTEL_486:
            processorType = "Intel 486";
            break;
        case PROCESSOR_INTEL_PENTIUM:
            processorType = "Intel Pentium";
            break;
        case 8664:
            processorType = "x64 (AMD or Intel 64-bit)";
            break;
        default:
            processorType = "Неизвестный тип процессора";
            break;
    }
    std::cout << "Тип процессора: " << processorType << std::endl;
    std::cout << "Гранулярность процессора: " << si.dwAllocationGranularity << " b" << std::endl;
    std::cout << "Уровень процессора: " << si.wProcessorLevel << " (Индикатор семейства процессоров)" << std::endl;
    WORD revision = si.wProcessorRevision;
    BYTE model = revision >> 8;
    BYTE stepping = revision & 0xFF;

    std::cout << "Ревизия процессора: " << "Модель: " << (int)model 
        << ", Версия ядра: " << (int)stepping << std::endl;
}

void showMemoryStatus() {
    MEMORYSTATUS memStatus;
    GlobalMemoryStatus(&memStatus);

    std::cout << "Состояние памяти:" << std::endl;
    std::cout << "Занято памяти: " << memStatus.dwMemoryLoad << "%" << std::endl;
    std::cout << "Всего физической памяти: " << memStatus.dwTotalPhys / (1024 * 1024) << " MB" << std::endl;
    std::cout << "Используемая физическая память: " << memStatus.dwAvailPhys / (1024 * 1024) << " MB" << std::endl;
    std::cout << "Всего виртуальной памяти: " << memStatus.dwTotalVirtual / (1024 * 1024) << " MB" << std::endl;
    std::cout << "Доступно виртуальной памяти: " << memStatus.dwAvailVirtual / (1024 * 1024) << " MB" << std::endl;
}

void queryMemoryRegion(void* address = 0x00000000) {
    if (address == 0x00000000) {
        std::cout << "Введите адрес памяти (в шестнадцатеричном формате, например 0x12345678): ";
        std::cin >> std::hex >> address;
    }

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(mbi))) {
        std::cout << "Базовый адрес: " << mbi.BaseAddress << "\n";
        std::cout << "Размер региона: " << mbi.RegionSize << "\n";
        std::cout << "Состояние: ";
        switch (mbi.State) {
            case MEM_COMMIT: std::cout << "COMMIT" << std::endl; break;
            case MEM_RESERVE: std::cout << "RESERVE" << std::endl; break;
            case MEM_FREE: std::cout << "FREE" << std::endl; break;
            case MEM_PRIVATE: std::cout << "PRIVATE" << std::endl; break;
            case MEM_MAPPED: std::cout << "MAPPED" << std::endl; break;
            case MEM_IMAGE: std::cout << "IMAGE" << std::endl; break;
            default: break;
        }
        std::cout << "Защита: ";
        switch (mbi.Protect) {
            case PAGE_EXECUTE: std::cout << "PAGE_EXECUTE" << std::endl; break;
            case PAGE_EXECUTE_READ: std::cout << "PAGE_EXECUTE_READ" << std::endl; break;
            case PAGE_EXECUTE_READWRITE: std::cout << "PAGE_EXECUTE_READWRITE" << std::endl; break;
            case PAGE_EXECUTE_WRITECOPY: std::cout << "PAGE_EXECUTE_WRITECOPY" << std::endl; break;
            case PAGE_NOACCESS: std::cout << "PAGE_NOACCESS" << std::endl; break ;
            case PAGE_READONLY: std::cout << "PAGE_READONLY" << std::endl; break ;
            case PAGE_READWRITE: std::cout << "PAGE_READWRITE" << std::endl; break;
            case PAGE_WRITECOPY: std::cout << "PAGE_WRITECOPY" << std::endl; break;
        default: break;
        }
    } else {
        std::cerr << "Не удалось выполнить запрос памяти.\n";
    }
}

void reserveAndCommitMemory(bool s) {
    void* address;

    std::cout << "Введите адрес (в шестнадцатеричном формате): ";
    std::cin >> std::hex >> address;

    void* reservedMemory;
    void* allocatedMemory;

    SIZE_T regionSize = GetDefaultRegionSize();
    if (s) {
        reservedMemory = VirtualAlloc(address, regionSize, MEM_RESERVE, PAGE_READWRITE);
        if (reservedMemory == NULL) {
            std::cout << "Ошибка во время резервирования памяти, Код ошибки: " << GetLastError() << std::endl;
            return;
        }
        std::cout << "Зарезервирован регион памяти по адресу: " << reservedMemory << std::endl;

        allocatedMemory = VirtualAlloc(reservedMemory, regionSize, MEM_COMMIT, PAGE_READWRITE);
    } else {
        allocatedMemory = VirtualAlloc(address, regionSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    }

    if (allocatedMemory == NULL) {
        std::cout << "Ошибка во время выделения памяти. Код ошибки: " << GetLastError() << std::endl;
        VirtualFree(reservedMemory, 0, MEM_RELEASE);
        return;
    }

    std::cout << "Выделен регион памяти по адресу: " << reservedMemory << std::endl;

    std::string symb;
    std::cout << "Хотите очистить выделенную память? (y/n) ";
    std::cin >> symb;
    if (symb == "y" || symb == "Y") {
        FreeAllocatedMemory(reservedMemory);
    } else {
        std::cout << "Память не очищены. Убедитесь что очистили ее позже во избежание утечек памяти." << std::endl;
    }
}

void reserveAndCommitMemoryAuto(bool s) {
    SIZE_T regionSize = GetDefaultRegionSize();

    void* reservedMemory;
    void* allocatedMemory;

    if (s) {
        reservedMemory = VirtualAlloc(NULL, regionSize, MEM_RESERVE, PAGE_READWRITE);
        if (reservedMemory == NULL) {
            std::cout << "Ошибка во время резервирования памяти, Код ошибки: " << GetLastError() << std::endl;
            return;
        }
        std::cout << "Зарезервирован регион памяти по адресу: " << reservedMemory << std::endl;

        allocatedMemory = VirtualAlloc(reservedMemory, regionSize, MEM_COMMIT, PAGE_READWRITE);
    } else {
        allocatedMemory = VirtualAlloc(NULL, regionSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    }

    if (allocatedMemory == NULL) {
        std::cout << "Ошибка во время резервирования памяти. Код ошибки: "  << GetLastError() << std::endl;
        return;
    }

    std::cout << "Регион памяти зарезервирован и выделен по адресу: " << allocatedMemory << std::endl;

    std::string symb;
    std::cout << "Хотите очистить выделенную память? (y/n) ";
    std::cin >> symb;
    if (symb == "y" || symb == "Y") {
        FreeAllocatedMemory(allocatedMemory);
    } else {
        std::cout << "Память не очищена. Убедитесь что очистили ее позже во избежание утечек памяти." << std::endl;
    }
}

void writeToMemory() {
    std::string data;
    void* address;

    std::cout << "Введите адрес (в шестнадцатеричном формате): ";
    std::cin >> std::hex >> address;
    std::cout << "Введите данные: ";
    std::cin >> data;

    size_t requiredSize = data.length() + 1;

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(mbi)) == 0) {
        std::cout << "Ошибка во время запроса за состоянием памяти. Код ошибки: " << GetLastError() << std::endl;
        return;
    }

    if (!(mbi.Protect & PAGE_READWRITE) && !(mbi.Protect & PAGE_EXECUTE_READWRITE)) {
        std::cout << "Ошибка: Память по адресу " << address
            << " не позволяет запись.\nТекущие флаги защиты: ";

        switch (mbi.Protect) {
            case PAGE_NOACCESS: std::cout << "PAGE_NOACCESS (No access)"; break;
            case PAGE_READONLY: std::cout << "PAGE_READONLY (Read-only)"; break;
            case PAGE_READWRITE: std::cout << "PAGE_READWRITE (Read/Write)"; break;
            case PAGE_WRITECOPY: std::cout << "PAGE_WRITECOPY (Write-copy)"; break;
            case PAGE_EXECUTE: std::cout << "PAGE_EXECUTE (Execute-only)"; break;
            case PAGE_EXECUTE_READ: std::cout << "PAGE_EXECUTE_READ (Execute/Read)"; break;
            case PAGE_EXECUTE_READWRITE: std::cout << "PAGE_EXECUTE_READWRITE (Execute/Read/Write)"; break;
            case PAGE_EXECUTE_WRITECOPY: std::cout << "PAGE_EXECUTE_WRITECOPY (Execute/Write-copy)"; break;
            default: std::cout << "Unknown protection code: " << mbi.Protect; break;
        }
        
        std::cout << "\nУчасток памяти не может быть изменен. Операция отменена." << std::endl;
        return;
    }

    std::cout << "Данные сохранены по адресу " << address << ": " << data << std::endl;
}


void protectMemory() {
    void* address;
    std::cout << "Введите адрес памяти (в шестнадцатеричном формате, например 0x12345678): ";
    std::cin >> std::hex >> address;

    SIZE_T regionSize = GetDefaultRegionSize();

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(mbi)) == 0) {
        std::cout << "Ошибка во время запроса за состоянием памяти. Код ошибки: " << GetLastError() << std::endl;
        return;
    }
    std::cout << "Текущеая защита до изменения: ";
    switch (mbi.Protect) {
        case PAGE_NOACCESS: std::cout << "PAGE_NOACCESS"; break;
        case PAGE_READONLY: std::cout << "PAGE_READONLY"; break;
        case PAGE_READWRITE: std::cout << "PAGE_READWRITE"; break;
        case PAGE_EXECUTE: std::cout << "PAGE_EXECUTE"; break;
        case PAGE_EXECUTE_READ: std::cout << "PAGE_EXECUTE_READ"; break;
        case PAGE_EXECUTE_READWRITE: std::cout << "PAGE_EXECUTE_READWRITE"; break;
        default: std::cout << "Unknown"; break;
    }
    std::cout << std::endl;

    std::cout << "1. PAGE_NOACCESS (access denied)" << std::endl;
    std::cout << "2. PAGE_READONLY (read-only)" << std::endl;
    std::cout << "3. PAGE_READWRITE (read and write)" << std::endl;
    std::cout << "4. PAGE_EXECUTE (execute code)" << std::endl;
    std::cout << "5. PAGE_EXECUTE_READ (execute and read)" << std::endl;
    std::cout << "6. PAGE_EXECUTE_READWRITE (execute, read, and write)" << std::endl;
    std::cout << "Choose the type of protection to set: ";
    
    int choice;
    std::cin >> choice;
    
    DWORD protection;
    switch (choice) {
        case 1: protection = PAGE_NOACCESS; break;
        case 2: protection = PAGE_READONLY; break;
        case 3: protection = PAGE_READWRITE; break;
        case 4: protection = PAGE_EXECUTE; break;
        case 5: protection = PAGE_EXECUTE_READ; break;
        case 6: protection = PAGE_EXECUTE_READWRITE; break;
        default:
            std::cout << "Ошибка: Неправильный выбор." << std::endl;
            return;
    }
    
    DWORD oldProtect;
    if (VirtualProtect(address, regionSize, protection, &oldProtect)) {
        std::cout << "Защита успешно установлена." << std::endl;
    } else {
        std::cout << "Ошибка во время установки защиты. Код ошибки: " << GetLastError() << std::endl;
        return;
    }
    
    std::string symb;
    std::cout << "Хотите посмотреть состояние памяти? (y/n) ";
    std::cin >> symb;
    if (symb == "y" || symb == "Y") {
        queryMemoryRegion(address);
    }
    
    std::cout << "Хотите очистить выделенную память? (y/n) ";
    std::cin >> symb;
    if (symb == "y" || symb == "Y") {
        void* memPtr = address;
        FreeAllocatedMemory(memPtr);
    } else {
        std::cout << "Память не очищена. Убедитесь что очистили ее позже во избежание утечек памяти" << std::endl;
    }
}

int main() {
    std::setlocale(LC_ALL, "");
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    std::wcin.imbue(std::locale());

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    int choice;
    while (true) {
        std::cout << "\n--- Меню ---\n";
        std::cout << "1. Информация о системе\n";
        std::cout << "2. Статус памяти\n";
        std::cout << "3. Запрос информации о регионе памяти\n";
        std::cout << "4. Раздельно зарезервировать и выделить память (автоматический адрес)\n";
        std::cout << "5. Раздельно арезервировать и выделить память (вручную)\n";
        std::cout << "6. Одновременно зарезервировать и выделить память (автоматический адрес)\n";
        std::cout << "7. Одновременно арезервировать и выделить память (вручную)\n";
        std::cout << "8. Записать в память по адресу\n";
        std::cout << "9. Установить и проверить защиту\n";
        std::cout << "0. Выход\n";
        std::cout << "Выберите пункт: ";
        
        std::wcin >> choice;

        switch (choice) {
            case 1: showSystemInfo(); break;
            case 2: showMemoryStatus(); break;
            case 3: queryMemoryRegion(); break;
            case 4: reserveAndCommitMemoryAuto(true); break;
            case 5: reserveAndCommitMemory(true); break;
            case 6: reserveAndCommitMemoryAuto(false); break;
            case 7: reserveAndCommitMemory(false); break;
            case 8: writeToMemory(); break;
            case 9: protectMemory(); break;
            case 0: return 0;
            default: std::cout << "Неверный вариант.\n";
        }
    }
}