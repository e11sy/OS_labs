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
     * Returns a vector of logical drive letters (e.g., C:\, D:\)
     * @return vector of drive strings
     */
    std::vector<std::wstring> listDrives();

    /**
     * Retrieves drive type for a given drive root path
     * @param drivePath Path like "C:\\"
     * @return Drive type (DRIVE_FIXED, DRIVE_REMOVABLE, etc.)
     */
    UINT getDriveType(const std::wstring& drivePath);

    /**
     * Retrieves volume information for a given drive
     * @param drivePath Root path of the drive
     * @param volumeName Output volume name
     * @param fsName Output file system name
     * @param serial Output volume serial number
     * @param maxCompLen Output max component length
     * @param fsFlags Output file system flags
     * @return true if successful, false otherwise
     */
    bool getVolumeInformation(
        const std::wstring& drivePath,
        std::wstring& volumeName,
        std::wstring& fsName,
        DWORD& serial,
        DWORD& maxCompLen,
        DWORD& fsFlags
    );

    /**
     * Retrieves free and total space for the specified drive
     * @param drivePath Drive root path
     * @param total Output total space in bytes
     * @param free Output free space in bytes
     * @return true if successful, false otherwise
     */
    bool getDiskFreeSpace(
        const std::wstring& drivePath,
        ULARGE_INTEGER& total,
        ULARGE_INTEGER& free
    );

    /**
     * Creates a directory at the given path
     * @param path Full directory path
     * @return true if the directory is created successfully, false otherwise
     */
    bool createDirectory(const std::wstring& path);

    /**
     * Removes the directory at the given path
     * @param path Full directory path
     * @return true if the directory is removed successfully, false otherwise
     */
    bool removeDirectory(const std::wstring& path);
};
