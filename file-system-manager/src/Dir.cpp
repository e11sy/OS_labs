#include "Dir.hpp"
#include <windows.h>
#include <iostream>

void DirHelper::listLogicalDrives() {
    DWORD drives = GetLogicalDrives();
    if (drives == 0) {
        std::cerr << "Failed to get logical drives." << std::endl;
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

    UINT type = GetDriveType(drive.c_str());
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

    if (!GetVolumeInformation(drive.c_str(), volName, MAX_PATH, &serialNum, &maxCompLen, &fsFlags, fsName, MAX_PATH)) {
        std::cerr << "Failed to get volume information." << std::endl;
        return;
    }

    std::cout << "Volume Name: " << volName << std::endl;
    std::cout << "File System: " << fsName << std::endl;

    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    if (!GetDiskFreeSpaceEx(drive.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
        std::cerr << "Failed to get disk free space." << std::endl;
        return;
    }

    std::cout << "Total Size: " << totalBytes.QuadPart / (1024 * 1024) << " MB" << std::endl;
    std::cout << "Free Space: " << totalFreeBytes.QuadPart / (1024 * 1024) << " MB" << std::endl;
}

void DirHelper::createDirectory(const std::string& path) {
    if (CreateDirectory(path.c_str(), NULL)) {
        std::cout << "Directory created successfully." << std::endl;
    } else {
        std::cerr << "Failed to create directory." << std::endl;
    }
}

void DirHelper::removeDirectory(const std::string& path) {
    if (RemoveDirectory(path.c_str())) {
        std::cout << "Directory removed successfully." << std::endl;
    } else {
        std::cerr << "Failed to remove directory." << std::endl;
    }
}
