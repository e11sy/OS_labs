// File.cpp
#include "File.hpp"
#include <iostream>
#include <windows.h>
#include <fileapi.h>

void FileHelper::createFile(const std::string& path) {
    HANDLE hFile = CreateFileA(
        path.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create file. Error: " << GetLastError() << std::endl;
    } else {
        std::cout << "File created successfully." << std::endl;
        CloseHandle(hFile);
    }
}

void FileHelper::copyFile(const std::string& source, const std::string& destination) {
    if (!CopyFileA(source.c_str(), destination.c_str(), TRUE)) {
        std::cerr << "Failed to copy file. Error: " << GetLastError() << std::endl;
    } else {
        std::cout << "File copied successfully." << std::endl;
    }
}

void FileHelper::moveFile(const std::string& source, const std::string& destination) {
    if (!MoveFileExA(source.c_str(), destination.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
        std::cerr << "Failed to move file. Error: " << GetLastError() << std::endl;
    } else {
        std::cout << "File moved successfully." << std::endl;
    }
}

void FileHelper::showAttributes(const std::string& path) {
    DWORD attributes = GetFileAttributesA(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "Failed to get file attributes. Error: " << GetLastError() << std::endl;
        return;
    }

    std::cout << "Attributes for " << path << ":\n";
    if (attributes & FILE_ATTRIBUTE_ARCHIVE) std::cout << "- Archive\n";
    if (attributes & FILE_ATTRIBUTE_HIDDEN) std::cout << "- Hidden\n";
    if (attributes & FILE_ATTRIBUTE_READONLY) std::cout << "- Read-only\n";
    if (attributes & FILE_ATTRIBUTE_SYSTEM) std::cout << "- System\n";

    HANDLE hFile = CreateFileA(
        path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file handle for time info. Error: " << GetLastError() << std::endl;
        return;
    }

    FILETIME creationTime, accessTime, writeTime;
    if (GetFileTime(hFile, &creationTime, &accessTime, &writeTime)) {
        SYSTEMTIME stUTC, stLocal;
        FileTimeToSystemTime(&creationTime, &stUTC);
        SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
        std::cout << "- Created: " << stLocal.wDay << "/" << stLocal.wMonth << "/" << stLocal.wYear << " "
                  << stLocal.wHour << ":" << stLocal.wMinute << std::endl;
    }

    CloseHandle(hFile);
}

void FileHelper::modifyAttributes(const std::string& path) {
    DWORD attributes = GetFileAttributesA(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "Failed to get file attributes. Error: " << GetLastError() << std::endl;
        return;
    }

    if (attributes & FILE_ATTRIBUTE_READONLY) {
        attributes &= ~FILE_ATTRIBUTE_READONLY;
        std::cout << "Removing read-only attribute...\n";
    } else {
        attributes |= FILE_ATTRIBUTE_READONLY;
        std::cout << "Adding read-only attribute...\n";
    }

    if (!SetFileAttributesA(path.c_str(), attributes)) {
        std::cerr << "Failed to modify file attributes. Error: " << GetLastError() << std::endl;
    } else {
        std::cout << "File attributes updated successfully." << std::endl;
    }
}
