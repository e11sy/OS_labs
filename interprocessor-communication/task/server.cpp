#include <windows.h>
#include <iostream>
#include "../shared/pipe.h"


int main() {
    HANDLE pipe = CreateNamedPipeA(
        PIPE_PATH,
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        MSG_BUFFER,
        MSG_BUFFER,
        0,
        nullptr
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        std::cout << "Pipe creation failed. Code: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "[SERVER]: Waiting for client..." << std::endl;

    HANDLE syncEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!syncEvent) {
        std::cout << "Failed to create event. Code: " << GetLastError() << std::endl;
        CloseHandle(pipe);
        return 1;
    }

    OVERLAPPED overlappedOp = {};
    overlappedOp.hEvent = syncEvent;

    BOOL connected = ConnectNamedPipe(pipe, &overlappedOp);
    if (!connected && GetLastError() != ERROR_IO_PENDING) {
        std::cout << "Connection failed. Code: " << GetLastError() << std::endl;
        CloseHandle(pipe);
        CloseHandle(syncEvent);
        return 1;
    }

    DWORD waitResult = WaitForSingleObject(syncEvent, INFINITE);
    if (waitResult != WAIT_OBJECT_0) {
        std::cout << "Waiting for connection failed. Code: " << GetLastError() << std::endl;
        CloseHandle(pipe);
        CloseHandle(syncEvent);
        return 1;
    }

    std::cout << "[SERVER]: Client connected." << std::endl;

    bool running = true;

    while (running) {
        std::cout << "\n---SERVER---\n1. Send message\n2. Disconnect\nEnter number: ";
        int selection;
        std::cin >> selection;
        std::cin.ignore();
        
        switch (selection) {
            case 1: {
                // Отправка сообщения клиенту
                std::string input;
                std::cout << "Enter message: ";
                std::getline(std::cin, input);

                DWORD writtenBytes = 0;
                ResetEvent(syncEvent);
                overlappedOp.Offset = overlappedOp.OffsetHigh = 0;

                // Асинхронная запись в канал
                BOOL writeResult = WriteFile(
                    pipe,
                    input.c_str(),
                    (DWORD)(input.length() + 1),
                    &writtenBytes,
                    &overlappedOp
                );

                if (!writeResult && GetLastError() != ERROR_IO_PENDING) {
                    std::cout << "Write failed. Code: " << GetLastError() << std::endl;
                }

                // Ждём завершения записи
                waitResult = WaitForSingleObject(syncEvent, INFINITE);
                if (waitResult != WAIT_OBJECT_0) {
                    std::cout << "Write wait failed. Code: " << GetLastError() << std::endl;
                } else {
                    std::cout << "[SERVER]: Message sent." << std::endl;
                }

                break;
            }

            case 2:
                std::cout << "Disconnecting server..." << std::endl;
                running = false;
                break;

            default:
                std::cout << "Invalid menu choice." << std::endl;
                break;
        }
    }

    // Завершение работы сервера
    DisconnectNamedPipe(pipe);
    CloseHandle(pipe);
    CloseHandle(syncEvent);

    std::cout << "[SERVER]: Disconnected and exiting." << std::endl;
    return 0;
}
