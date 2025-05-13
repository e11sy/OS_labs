#pragma once
#include <string>
#include <windows.h>

/**
 * Class for file operations: creation, copying, moving, attribute management
 *
 * Provides an interface for file-related operations using the Win32 API,
 * including file creation, copying, moving, and working with attributes and timestamps.
 */
class FileHelper {
public:
    /**
     * Creates a file at the specified path
     * @param path Full path to the file to be created
     * @return true if the file is successfully created, false otherwise
     */
    bool createFile(const std::wstring& path);

    /**
     * Copies a file from source to destination
     * @param source Source file path
     * @param dest Destination file path
     * @param failIfExists If true, the operation will fail if the destination file already exists
     * @return true if the copy is successful, false otherwise
     */
    bool copyFile(const std::wstring& source, const std::wstring& dest, bool failIfExists = true);

    /**
     * Moves a file from source to destination
     * @param source Source file path
     * @param dest Destination file path
     * @param flags Move operation flags (e.g., MOVEFILE_REPLACE_EXISTING)
     * @return true if the move is successful, false otherwise
     */
    bool moveFile(const std::wstring& source, const std::wstring& dest, DWORD flags = 0);

    /**
     * Retrieves the attributes of a file
     * @param path Path to the file
     * @return DWORD representing the file attributes
     */
    DWORD getFileAttributes(const std::wstring& path);

    /**
     * Sets the attributes of a file
     * @param path Path to the file
     * @param attrs New attributes to set
     * @return true if the attributes were successfully set, false otherwise
     */
    bool setFileAttributes(const std::wstring& path, DWORD attrs);

    /**
     * Retrieves the file's timestamp information
     * @param path Path to the file
     * @param creation Receives the creation time
     * @param access Receives the last access time
     * @param write Receives the last write time
     * @return true if the operation is successful, false otherwise
     */
    bool getFileTimes(const std::wstring& path, FILETIME& creation, FILETIME& access, FILETIME& write);

    /**
     * Sets the file's timestamp information
     * @param path Path to the file
     * @param creation New creation time
     * @param access New last access time
     * @param write New last write time
     * @return true if the operation is successful, false otherwise
     */
    bool setFileTimes(const std::wstring& path, const FILETIME& creation, const FILETIME& access, const FILETIME& write);
};
