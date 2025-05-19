#include "Dir.hpp"
#include <windows.h>
#include <iostream>
#include <vector>

void DirHelper::listLogicalDrives() {
    DWORD drives = GetLogicalDrives();
    if (drives == 0) {
        std::wcerr << "Failed to get logical drives." << std::endl;
        return;
    }

    std::cout << "Logical Drives:" << std::endl;
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        if (drives & (1 << (letter - 'A')))
            std::cout << letter << ":\\" << std::endl;
    }
}

void DirHelper::showDriveInformation(const std::string& drive) {
    char fsName[MAX_PATH + 1] = { 0 };
    char volName[MAX_PATH + 1] = { 0 };
    DWORD serialNum = 0, maxCompLen = 0, fsFlags = 0;

    UINT type = GetDriveTypeA(drive.c_str());
    std::cout << "Drive type: ";
    switch (type) {
        case DRIVE_FIXED: std::cout << "Fixed"; break;
        case DRIVE_REMOVABLE: std::cout << "Removable"; break;
        case DRIVE_CDROM: std::cout << "CD-ROM"; break;
        case DRIVE_REMOTE: std::cout << "Remote"; break;
        case DRIVE_RAMDISK: std::cout << "RAM Disk"; break;
        default: std::cout << "Unknown"; break;
    }
    std::cout << std::endl;

    if (!GetVolumeInformationA(drive.c_str(), volName, MAX_PATH, &serialNum, &maxCompLen, &fsFlags, fsName, MAX_PATH)) {
        std::wcerr << "Failed to get volume information." << std::endl;
        return;
    }

    std::cout << "Volume Name: " << volName << std::endl;
    std::cout << "Serial Number: " << serialNum << std::endl;
    std::cout << "Max File Name Length: " << maxCompLen << std::endl; 
    std::cout << "File System: " << fsName << std::endl;

    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    if (!GetDiskFreeSpaceExA(drive.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
        std::wcerr << "Failed to get disk free space." << std::endl;
        return;
    }

    std::cout << "Total Size: " << totalBytes.QuadPart / (1024 * 1024 * 1024) << " GB" << std::endl;
    std::cout << "Free Space: " << totalFreeBytes.QuadPart / (1024 * 1024 * 1024) << " GB" << std::endl;
}

void DirHelper::createDirectory(const std::string& path) {
    if (CreateDirectoryA(path.c_str(), NULL)) {
        std::cout << "Directory created successfully." << std::endl;
    } else {
        std::wcerr << "Failed to create directory." << std::endl;
    }
}

void DirHelper::removeDirectory(const std::string& path) {
    if (RemoveDirectoryA(path.c_str())) {
        std::cout << "Directory removed successfully." << std::endl;
    } else {
        std::wcerr << "Failed to remove directory." << std::endl;
    }
}

// Missing implementations (wstring versions)

std::vector<std::wstring> DirHelper::listDrives() {
    std::vector<std::wstring> drives;
    DWORD driveMask = GetLogicalDrives();
    for (wchar_t letter = L'A'; letter <= L'Z'; ++letter) {
        if (driveMask & (1 << (letter - L'A'))) {
            std::wstring drive = std::wstring(1, letter) + L":\\";
            drives.push_back(drive);
        }
    }
    return drives;
}

UINT DirHelper::getDriveType(const std::wstring& drivePath) {
    return GetDriveTypeW(drivePath.c_str());
}

bool DirHelper::getVolumeInformation(
    const std::wstring& drivePath,
    std::wstring& volumeName,
    std::wstring& fsName,
    DWORD& serial,
    DWORD& maxCompLen,
    DWORD& fsFlags
) {
    wchar_t volNameBuf[MAX_PATH];
    wchar_t fsNameBuf[MAX_PATH];

    BOOL success = GetVolumeInformationW(
        drivePath.c_str(),
        volNameBuf,
        MAX_PATH,
        &serial,
        &maxCompLen,
        &fsFlags,
        fsNameBuf,
        MAX_PATH
    );

    if (success) {
        volumeName = volNameBuf;
        fsName = fsNameBuf;
    }

    return success;
}

bool DirHelper::getDiskFreeSpace(
    const std::wstring& drivePath,
    ULARGE_INTEGER& total,
    ULARGE_INTEGER& free
) {
    ULARGE_INTEGER available;
    return GetDiskFreeSpaceExW(drivePath.c_str(), &available, &total, &free);
}

bool DirHelper::createDirectory(const std::wstring& path) {
    return CreateDirectoryW(path.c_str(), NULL);
}

bool DirHelper::removeDirectory(const std::wstring& path) {
    return RemoveDirectoryW(path.c_str());
}
