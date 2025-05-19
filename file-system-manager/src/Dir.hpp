#pragma once
#include <string>
#include <vector>
#include <windows.h>

/**
 * Class for directory and drive operations
 *
 * Provides methods to manage logical drives, directories, and disk information
 * using the Win32 API.
 */
class DirHelper {
public:
    /**
     * Lists all logical drives and prints them to stdout.
     */
    static void listLogicalDrives();

    /**
     * Shows information about a specific drive (e.g., C:\).
     * @param drive The drive path as a string.
     */
    static void showDriveInformation(const std::string& drive);

    /**
     * Creates a directory at the given path.
     * @param path The directory path.
     */
    static void createDirectory(const std::string& path);

    /**
     * Removes a directory at the given path.
     * @param path The directory path.
     */
    static void removeDirectory(const std::string& path);

    // Methods declared in .hpp but not implemented in .cpp:
    static std::vector<std::wstring> listDrives();
    static UINT getDriveType(const std::wstring& drivePath);
    static bool getVolumeInformation(
        const std::wstring& drivePath,
        std::wstring& volumeName,
        std::wstring& fsName,
        DWORD& serial,
        DWORD& maxCompLen,
        DWORD& fsFlags
    );
    static bool getDiskFreeSpace(
        const std::wstring& drivePath,
        ULARGE_INTEGER& total,
        ULARGE_INTEGER& free
    );
    static bool createDirectory(const std::wstring& path);
    static bool removeDirectory(const std::wstring& path);
};
