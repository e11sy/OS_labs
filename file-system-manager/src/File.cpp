#include "File.hpp"
#include <windows.h>
#include <iostream>
#include <map>
#include <sstream>
#include <iomanip>

bool FileHelper::createFile(const std::wstring& path) {
    HANDLE hFile = CreateFileW(
        path.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::wcerr << "Failed to create file. Error: " << GetLastError() << std::endl;
        return false;
    }

    CloseHandle(hFile);
    return true;
}

bool FileHelper::copyFile(const std::wstring& source, const std::wstring& destination) {
    if (!CopyFileW(source.c_str(), destination.c_str(), TRUE)) {
        std::wcerr << "Failed to copy file. Error: " << GetLastError() << std::endl;
        return false;
    }
    return true;
}

bool FileHelper::moveFile(const std::wstring& source, const std::wstring& destination, DWORD flags) {
    if (!MoveFileExW(source.c_str(), destination.c_str(), flags)) {
        std::wcerr << "Failed to move file. Error: " << GetLastError() << std::endl;
        return false;
    }
    return true;
}

DWORD FileHelper::getFileAttributes(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        std::wcerr << "Failed to get file attributes. Error: " << GetLastError() << std::endl;
    }
    return attributes;
}

bool FileHelper::setFileAttributes(const std::wstring& path, DWORD attributes) {
    if (!SetFileAttributesW(path.c_str(), attributes)) {
        std::wcerr << L"Failed to set file attributes. Error: " << GetLastError() << std::endl;
        return false;
    }
    return true;
}

bool FileHelper::setFileAttributesInteractive(const std::wstring& path) {
    std::map<std::wstring, DWORD> attrMap = {
        {L"READONLY", FILE_ATTRIBUTE_READONLY},
        {L"HIDDEN", FILE_ATTRIBUTE_HIDDEN},
        {L"SYSTEM", FILE_ATTRIBUTE_SYSTEM},
        {L"DIRECTORY", FILE_ATTRIBUTE_DIRECTORY},
        {L"ARCHIVE", FILE_ATTRIBUTE_ARCHIVE},
        {L"DEVICE", FILE_ATTRIBUTE_DEVICE},
        {L"NORMAL", FILE_ATTRIBUTE_NORMAL},
        {L"TEMPORARY", FILE_ATTRIBUTE_TEMPORARY},
        {L"SPARSE_FILE", FILE_ATTRIBUTE_SPARSE_FILE},
        {L"REPARSE_POINT", FILE_ATTRIBUTE_REPARSE_POINT},
        {L"COMPRESSED", FILE_ATTRIBUTE_COMPRESSED},
        {L"OFFLINE", FILE_ATTRIBUTE_OFFLINE},
        {L"NOT_CONTENT_INDEXED", FILE_ATTRIBUTE_NOT_CONTENT_INDEXED},
        {L"ENCRYPTED", FILE_ATTRIBUTE_ENCRYPTED},
        {L"INTEGRITY_STREAM", FILE_ATTRIBUTE_INTEGRITY_STREAM},
        {L"VIRTUAL", FILE_ATTRIBUTE_VIRTUAL},
        {L"NO_SCRUB_DATA", FILE_ATTRIBUTE_NO_SCRUB_DATA},
    };

    std::wcout << L"Available attributes:\n";
    for (const auto& [name, val] : attrMap) {
        std::wcout << L"  " << std::left << std::setw(20) << name
                   << L" = 0x" << std::hex << val << std::endl;
    }

    std::wcout << L"\nEnter attributes (hex values separated by space, e.g., 0x1 0x20): ";
    std::wstring line;
    std::getline(std::wcin, line);

    std::wstringstream ss(line);
    std::wstring token;
    DWORD finalAttrs = 0;

    while (ss >> token) {
        try {
            DWORD val = std::stoul(token, nullptr, 0);
            finalAttrs |= val;
        } catch (...) {
            std::wcerr << L"Invalid attribute value: " << token << std::endl;
            return false;
        }
    }

    return setFileAttributes(path, finalAttrs);
}

