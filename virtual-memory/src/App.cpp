#include <windows.h>
#include <iostream>
#include <string>
#include <locale>


void showSystemInfo() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    std::cout << "Архитектура процессора: " << si.wProcessorArchitecture << "\n";
    std::cout << "Количество процессоров: " << si.dwNumberOfProcessors << "\n";
    std::cout << "Размер страницы: " << si.dwPageSize << "\n";
    std::cout << "Минимальный адрес приложения: " << si.lpMinimumApplicationAddress << "\n";
    std::cout << "Максимальный адрес приложения: " << si.lpMaximumApplicationAddress << "\n";
}

void showMemoryStatus() {
    MEMORYSTATUS memStatus;
    memStatus.dwLength = sizeof(memStatus);
    GlobalMemoryStatus(&memStatus);

    std::cout << "Использование памяти: " << memStatus.dwMemoryLoad << "%\n";
    std::cout << "Всего физической памяти: " << memStatus.dwTotalPhys << "\n";
    std::cout << "Доступно физической памяти: " << memStatus.dwAvailPhys << "\n";
}

void queryMemoryRegion() {
    void* address;
    std::cout << "Введите адрес памяти (в шестнадцатеричном формате, например 0x12345678): ";
    std::cin >> std::hex >> address;

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(mbi))) {
        std::cout << "Базовый адрес: " << mbi.BaseAddress << "\n";
        std::cout << "Размер региона: " << mbi.RegionSize << "\n";
        std::cout << "Состояние: " << mbi.State << "\n";
        std::cout << "Защита: " << mbi.Protect << "\n";
    } else {
        std::cerr << "Не удалось выполнить запрос памяти.\n";
    }
}

void reserveAndCommitMemory(bool manualAddress) {
    LPVOID base = nullptr;
    if (manualAddress) {
        std::cout << "Введите базовый адрес (в шестнадцатеричном формате): ";
        std::cin >> std::hex >> base;
    }

    LPVOID region = VirtualAlloc(base, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (region)
        std::cout << "Память выделена по адресу: " << region << "\n";
    else
        std::cerr << "Выделение памяти не удалось.\n";
}

void writeToMemory() {
    void* address;
    int value;
    std::cout << "Введите адрес для записи (в шестнадцатеричном формате): ";
    std::cin >> std::hex >> address;
    std::cout << "Введите целочисленное значение для записи: ";
    std::cin >> std::dec >> value;

    if (!IsBadWritePtr(address, sizeof(int))) {
        *((int*)address) = value;
        std::cout << "Успешно записано " << value << " по адресу " << address << "\n";
    } else {
        std::cerr << "Неверный адрес памяти или доступ на запись запрещён.\n";
    }
}

void protectMemory() {
    void* address;
    SIZE_T size;
    DWORD oldProtect;

    std::cout << "Введите адрес (в шестнадцатеричном формате): ";
    std::cin >> std::hex >> address;
    std::cout << "Введите размер (в байтах): ";
    std::cin >> std::dec >> size;

    if (VirtualProtect(address, size, PAGE_NOACCESS, &oldProtect)) {
        std::cout << "Защита изменена на PAGE_NOACCESS.\n";
        std::cout << "Пытаемся прочитать данные после изменения защиты...\n";

        if (IsBadReadPtr(address, sizeof(int))) {
            std::cout << "Чтение заблокировано, как и ожидалось (память недоступна).\n";
        } else {
            int value = *((int*)address);
            std::cout << "Неожиданно: прочитано значение = " << value << "\n";
        }

        // Восстанавливаем прежнюю защиту
        VirtualProtect(address, size, oldProtect, &oldProtect);
    } else {
        std::cerr << "Не удалось изменить защиту.\n";
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
        std::cout << "4. Зарезервировать и выделить память (автоматический адрес)\n";
        std::cout << "5. Зарезервировать и выделить память (вручную)\n";
        std::cout << "6. Записать в память\n";
        std::cout << "7. Установить и проверить защиту\n";
        std::cout << "0. Выход\n";
        std::cout << "Выберите пункт: ";
        
        std::wcin >> choice;

        switch (choice) {
            case 1: showSystemInfo(); break;
            case 2: showMemoryStatus(); break;
            case 3: queryMemoryRegion(); break;
            case 4: reserveAndCommitMemory(false); break;
            case 5: reserveAndCommitMemory(true); break;
            case 6: writeToMemory(); break;
            case 7: protectMemory(); break;
            case 0: return 0;
            default: std::cout << "Неверный вариант.\n";
        }
    }
}