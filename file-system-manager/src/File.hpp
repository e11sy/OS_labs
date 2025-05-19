#pragma once
#include <string>
#include <windows.h>

/**
 * FileHelper provides utility methods for file operations
 * using wide-character (UTF-16) Windows API.
 */
class FileHelper {
public:
    /**
     * Creates a new file at the specified path.
     * @param path Path where the file should be created.
     * @return true if successful, false otherwise.
     */
    bool createFile(const std::wstring& path);

    /**
     * Copies a file from source to destination.
     * @param source Source file path.
     * @param destination Destination file path.
     * @return true if successful, false otherwise.
     */
    bool copyFile(const std::wstring& source, const std::wstring& destination);

    /**
     * Moves a file from source to destination with flags.
     * @param source Source file path.
     * @param destination Destination file path.
     * @param flags Flags for the move operation.
     * @return true if successful, false otherwise.
     */
    bool moveFile(const std::wstring& source, const std::wstring& destination, DWORD flags);

    /**
     * Gets file attributes for the given path.
     * @param path File path.
     * @return Attributes as a DWORD. INVALID_FILE_ATTRIBUTES on failure.
     */
    DWORD getFileAttributes(const std::wstring& path);

    /**
     * Sets file attributes for the given path.
     * @param path File path.
     * @param attributes Attribute flags to set.
     * @return true if successful, false otherwise.
     */
    bool setFileAttributes(const std::wstring& path, DWORD attributes);

    /**
     * Sets creates interactive attr changer.
     * @param path File path.
     * @return true if successful, false otherwise.
     */
    bool setFileAttributesInteractive(const std::wstring& path);
};
