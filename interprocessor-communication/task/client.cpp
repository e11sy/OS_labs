#include <windows.h>
#include <iostream>
#include "../shared/pipe.h"

// Функция обратного вызова, вызывается при завершении асинхронного чтения
VOID CALLBACK OnReadComplete(
    DWORD errorCode,
    DWORD bytesRead,
    LPOVERLAPPED overlapped
) {
    if (errorCode != 0) {
        std::cout << "Read failed. Code: " << errorCode << std::endl;
        return;
    }

    char* data = (char*)overlapped->hEvent;
    std::cout << "\n[CLIENT]: Message from server: " << data << std::endl;
}

int main() {
    // Подключение к существующему каналу
    HANDLE pipeHandle = CreateFileA(
        PIPE_PATH,
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        nullptr
    );

    if (pipeHandle == INVALID_HANDLE_VALUE) {
        std::cout << "Unable to connect to named pipe. Error: " << GetLastError() << std::endl;
        return 1;
    }

    DWORD mode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(pipeHandle, &mode, nullptr, nullptr)) {
        std::cout << "Failed to set pipe mode. Error: " << GetLastError() << std::endl;
        CloseHandle(pipeHandle);
        return 1;
    }

    std::cout << "[CLIENT]: Connected to server." << std::endl;

    HANDLE ioEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!ioEvent) {
        std::cout << "Event creation failed. Code: " << GetLastError() << std::endl;
        CloseHandle(pipeHandle);
        return 1;
    }

    char inputBuffer[MSG_BUFFER];
    OVERLAPPED asyncIO = {};
    asyncIO.hEvent = (HANDLE)inputBuffer;

    bool running = true;

while (running) {
    // Меню клиента
    std::cout << "\n---CLIENT---\n1. Receive message\n2. Exit\nEnter number: ";
    int selection;
    std::cin >> selection;
    std::cin.ignore();

    switch (selection) {
        case 1: {
            // Асинхронное чтение из канала
            BOOL readOk = ReadFileEx(
                pipeHandle,
                inputBuffer,
                MSG_BUFFER,
                &asyncIO,
                OnReadComplete
            );

            if (!readOk) {
                std::cout << "ReadFileEx failed. Code: " << GetLastError() << std::endl;
            }

            // Ожидаем завершения асинхронной операции
            SleepEx(INFINITE, TRUE);
            break;
        }

        case 2:
            std::cout << "Disconnecting client..." << std::endl;
            running = false;
            break;

        default:
            std::cout << "Unknown option. Try again." << std::endl;
            break;
    }
}

    // Завершение клиента
    CloseHandle(pipeHandle);
    CloseHandle(ioEvent);

    std::cout << "[CLIENT]: Session ended." << std::endl;
    return 0;
}
