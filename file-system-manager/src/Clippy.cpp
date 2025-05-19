#include "File.hpp"
#include "Dir.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <locale>
#include <codecvt>
#define CLS system("CLS")

// Helper to convert string to wstring
std::wstring to_wstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

// Clippy messages
void clippySay(const std::string& message) {
    std::cout << "\n🤖 Clippy says: " << message << "\n";
}

void clippySayRandom() {
    const std::string funComments[] = {
        "Интересный выбор... Возможно, он принесёт немного хаоса в этот мир.",
        "Ну, если ты на 100% уверен, что это не лишнее...",
        "Никто не ожидал такого развития событий.",
        "Всегда приятно делать вид, что знаешь, что происходит.",
        "Каждая твоя операция — как новый эпизод в сериале.",
        "Это смело.",
        "Ладно я потом все это разберу сам.",
        "Ну, делай что хочешь, никто не судит.",
    };

    int idx = rand() % 10;
    clippySay(funComments[idx]);
}

class App {
private:
    DirHelper dirHelper;
    FileHelper fileHelper;

public:
    void run();
    void handleInput();

    void listDrives();
    void showDriveInfo();
    void createDirectory();
    void removeDirectory();
    void createFile();
    void copyFile();
    void moveFile();
    void showFileAttributes();
    void modifyFileAttributes();
};

void App::run() {
    while (true) {
        std::cout << "\nFile Manager Menu:\n"
                  << "1. List Drives\n"
                  << "2. Show Drive Info\n"
                  << "3. Create Directory\n"
                  << "4. Remove Directory\n"
                  << "5. Create File\n"
                  << "6. Copy File\n"
                  << "7. Move File\n"
                  << "8. Show File Attributes\n"
                  << "9. Modify File Attributes\n"
                  << "0. Exit\n"
                  << "Enter your choice: ";
        handleInput();
    }
}

void App::handleInput() {
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    switch (choice) {
        case 1: listDrives(); break;
        case 2: showDriveInfo(); break;
        case 3: createDirectory(); break;
        case 4: removeDirectory(); break;
        case 5: createFile(); break;
        case 6: copyFile(); break;
        case 7: moveFile(); break;
        case 8: showFileAttributes(); break;
        case 9: modifyFileAttributes(); break;
        case 0:
            clippySay("Exiting... Until next time!");
            exit(0);
        default:
            clippySay("Oops, that's not a valid choice! Try again.");
    }

    clippySayRandom();
}

void App::listDrives() {
    auto drives = dirHelper.listDrives();
    std::wcout << L"Available Drives:\n";
    for (const auto& drive : drives) {
        std::wcout << L"  " << drive << L"\n";
    }
}

void App::showDriveInfo() {
    std::string drive;
    std::cout << "Enter drive (e.g., C:\\): ";
    std::getline(std::cin, drive);

    std::wstring volName, fsName;
    DWORD serial, maxLen, fsFlags;
    dirHelper.showDriveInformation(drive);
}

void App::createDirectory() {
    std::string input;
    std::cout << "Enter directory path to create: ";
    std::getline(std::cin, input);
    std::wstring path = to_wstring(input);

    if (dirHelper.createDirectory(path)) {
        clippySay("Directory created successfully.");
    } else {
        clippySay("Failed to create directory.");
    }
}

void App::removeDirectory() {
    std::string input;
    std::cout << "Enter directory path to remove: ";
    std::getline(std::cin, input);
    std::wstring path = to_wstring(input);

    if (dirHelper.removeDirectory(path)) {
        clippySay("Directory removed successfully.");
    } else {
        clippySay("Failed to remove directory.");
    }
}

void App::createFile() {
    std::string input;
    std::cout << "Enter file path to create: ";
    std::getline(std::cin, input);
    std::wstring path = to_wstring(input);

    if (fileHelper.createFile(path)) {
        clippySay("Nice, another file added to the chaos!");
    } else {
        clippySay("Oops, file creation failed.");
    }
}

void App::copyFile() {
    std::string src, dest;
    std::cout << "Enter source file path: ";
    std::getline(std::cin, src);
    std::cout << "Enter destination file path: ";
    std::getline(std::cin, dest);

    if (fileHelper.copyFile(to_wstring(src), to_wstring(dest))) {
        clippySay("I hope you didn't just copy an empty file... did you?");
    } else {
        clippySay("Something went wrong with the copy.");
    }
}

void App::moveFile() {
    std::string src, dest;
    std::cout << "Enter source file path: ";
    std::getline(std::cin, src);
    std::cout << "Enter destination file path: ";
    std::getline(std::cin, dest);

    if (fileHelper.moveFile(to_wstring(src), to_wstring(dest), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
        clippySay("Like a file ninja... moved without a trace.");
    } else {
        clippySay("Move operation failed. The file resisted!");
    }
}

void App::showFileAttributes() {
    std::string input;
    std::cout << "Enter file path to inspect: ";
    std::getline(std::cin, input);

    DWORD attrs = fileHelper.getFileAttributes(to_wstring(input));
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        clippySay("Failed to get file attributes.");
        return;
    }

    std::cout << "Raw File Attributes: 0x" << std::hex << attrs << std::dec << "\n";

    std::cout << "Attributes:\n";
    if (attrs & FILE_ATTRIBUTE_READONLY)        std::cout << "- READONLY\n";
    if (attrs & FILE_ATTRIBUTE_HIDDEN)          std::cout << "- HIDDEN\n";
    if (attrs & FILE_ATTRIBUTE_SYSTEM)          std::cout << "- SYSTEM\n";
    if (attrs & FILE_ATTRIBUTE_DIRECTORY)       std::cout << "- DIRECTORY\n";
    if (attrs & FILE_ATTRIBUTE_ARCHIVE)         std::cout << "- ARCHIVE\n";
    if (attrs & FILE_ATTRIBUTE_NORMAL)          std::cout << "- NORMAL\n";
    if (attrs & FILE_ATTRIBUTE_TEMPORARY)       std::cout << "- TEMPORARY\n";
    if (attrs & FILE_ATTRIBUTE_OFFLINE)         std::cout << "- OFFLINE\n";
    if (attrs & FILE_ATTRIBUTE_ENCRYPTED)       std::cout << "- ENCRYPTED\n";
    if (attrs & FILE_ATTRIBUTE_COMPRESSED)      std::cout << "- COMPRESSED\n";
    if (attrs & FILE_ATTRIBUTE_REPARSE_POINT)   std::cout << "- REPARSE_POINT\n";
    if (attrs & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED) std::cout << "- NOT_CONTENT_INDEXED\n";
}


void App::modifyFileAttributes() {
    std::string input;
    std::cout << "Enter file path to modify: ";
    std::getline(std::cin, input);
    std::wstring path = to_wstring(input);

    if (fileHelper.setFileAttributesInteractive(path)) {
        clippySay("Attributes updated. Let's pretend that was on purpose.");
    } else {
        clippySay("Changing attributes failed. Try threatening the file?");
    }
}

int main() {
    std::setlocale(LC_ALL, "");

    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    srand(static_cast<unsigned int>(time(nullptr)));
    App app;
    app.run();
    return 0;
}  