#pragma comment (lib, "winmm.lib")
#include <windows.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

void* initBuffer(int size, bool read) {
    // Открытие/создание файла buffer.txt
    HANDLE file = CreateFileA(
        "buffer.txt",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file == INVALID_HANDLE_VALUE) {
        std::cout << "Error opening buffer.txt: " << GetLastError() << std::endl;
        system("pause");
        exit(1);
    }

    // Открытие/создание отображения файла в память
    HANDLE buffer = OpenFileMappingA(read ? FILE_MAP_READ : FILE_MAP_WRITE, FALSE, "buffer");
    if (buffer == NULL) {
        if (!read) {
            buffer = CreateFileMappingA(file, NULL, PAGE_READWRITE, 0, size, "buffer");
            if (buffer == NULL) {
                std::cout << "Error creating file mapping: " << GetLastError() << std::endl;
                system("pause");
                exit(1);
            }
        } else {
            std::cout << "Reader: file mapping not found, exiting." << std::endl;
            system("pause");
            exit(1);
        }
    }

    // Проецирование отображения в адресное пространство
    void* buffAddress = MapViewOfFile(buffer, read ? FILE_MAP_READ : FILE_MAP_WRITE, 0, 0, size);
    if (buffAddress == NULL) {
        std::cout << "Error mapping view of file: " << GetLastError() << std::endl;
        system("pause");
        exit(1);
    }

    // Блокировка страниц буфера в оперативной памяти
    if (!VirtualLock(buffAddress, size)) {
        std::cout << "Warning: VirtualLock failed: " << GetLastError() << std::endl;
    }

    return buffAddress;
}



// Инициализация семафоров
void initSemaphores(HANDLE* arr, int numOfPages, int state, const char* baseName) {
    for (int i = 0; i < numOfPages; i++) {
        char name[32];
        sprintf_s(name, "%s%d", baseName, i);

        // Открытие существующего семафора или создание нового
        arr[i] = OpenSemaphoreA(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, name);
        if (arr[i] == NULL) {
            arr[i] = CreateSemaphoreA(NULL, state, 1, name);
            if (arr[i] == NULL) {
                std::cout << "Error creating semaphore: " << name << " | Code: " << GetLastError() << std::endl;
                exit(1);
            }
        }
    }
}


int main(int argc, char* argv[]) {
    std::cout << "PID: " << GetCurrentProcessId() << " started." << std::endl;

    system("cls");
    // Инициализация генератора случайных чисел
    srand(static_cast<unsigned>(time(NULL)));

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    const int pages = 3 + 1 + 4 + 2 + 7; // номер билета 331427
    const int pageSize = sysInfo.dwPageSize;
    const int buffSize = pageSize * pages;

    // Определение типа процесса
    bool reader = (argc > 1 && strcmp(argv[1], "reader") == 0);
    std::cout << (reader ? "Reader App" : "Writer App") << std::endl;

    // Буфер для чтения или записи страницы
    char* message = new char[pageSize]();
    if (!reader) memset(message, 97 + rand() % 26, pageSize);

    // Открытие журнала для записи действий
    std::fstream journal(reader ? "ReaderJournal.txt" : "WriterJournal.txt", std::fstream::app);

    if (!journal.is_open()) {
        std::cout << "Failed to open journal file" << std::endl;
        return 1;
    }
    // Инициализация буфера
    void* buffAddress = initBuffer(buffSize, reader);

    // Инициализация семафоров
    HANDLE* sr = new HANDLE[pages];
    initSemaphores(sr, pages, 0, "reader");

    HANDLE* sw = new HANDLE[pages];
    initSemaphores(sw, pages, 1, "writer");

    HANDLE* waitS = reader ? sr : sw;
    HANDLE* releaseS = reader ? sw : sr;

    std::cout << std::setw(10) << "ADDRESS" << " | " << std::setw(10) << "PAGE NUMBER" << std::endl
         << "-----------+---------------" << std::endl;
    journal << std::endl << std::setw(10) << "PROCESS ID" << " | "
            << std::setw(10) << "TIME" << " | "
            << std::setw(12) << "STATE" << " | "
            << std::setw(10) << "PAGE NUMBER" << std::endl
            << "-----------+------------+--------------+------------" << std::endl;

    // Основной цикл чтения или записи
    while (true) {
        journal << std::setw(10) << GetCurrentProcessId() << " | "
                << std::setw(10) << timeGetTime() << " | "
                << std::setw(12) << "Wait" << " | " << std::endl;

        DWORD curPage = WaitForMultipleObjects(pages, waitS, FALSE, INFINITE);
        if (curPage == WAIT_FAILED) {
            DWORD err = GetLastError();
            std::cout << "WaitForMultipleObjects failed! Error: " << err << std::endl;
            system("pause");
            return 1;
        }
        if (curPage >= WAIT_OBJECT_0 && curPage < WAIT_OBJECT_0 + pages) {
            curPage = curPage - WAIT_OBJECT_0; // реальный номер страницы
        } else {
            std::cout << "Unexpected Wait result: " << curPage << std::endl;
            system("pause");
            return 1;
        }

        journal << std::setw(10) << GetCurrentProcessId() << " | "
                << std::setw(10) << timeGetTime() << " | "
                << std::setw(12) << ("Begin") << " | "
                << std::setw(10) << curPage << std::endl;

        std::cout << std::setw(10) << (void*)((char*)buffAddress + curPage * pageSize) << " | "
             << std::setw(5) << curPage << std::endl;

        if (reader)
            memcpy(message, (char*)buffAddress + curPage * pageSize, pageSize);
        else
            memcpy((char*)buffAddress + curPage * pageSize, message, pageSize);

        Sleep(500 + rand() % 1000);

        journal << std::setw(10) << GetCurrentProcessId() << " | "
                << std::setw(10) << timeGetTime() << " | "
                << std::setw(12) << ("End") << " | "
                << std::setw(10) << curPage << std::endl;

        ReleaseSemaphore(releaseS[curPage], 1, NULL);
    }

    return 0;
}
