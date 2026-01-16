#include <filesystem>
#include <vector>
#include <random>
#include <iostream>
#include <windows.h>
#include <sstream>

namespace fs = std::filesystem;

std::string normalizeExtension(std::string ext)
{
    if (ext.empty()) return ext;
    if (ext[0] != '.') ext = "." + ext;
    for (auto& c : ext) c = tolower(c);
    return ext;
}

std::vector<std::string> splitExtensions(const std::string& input)
{
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string item;

    while (std::getline(ss, item, ',')) {
        if (!item.empty()) {
            result.push_back(normalizeExtension(item));
        }
    }
    return result;
}

bool matchesType(const fs::path& p, const std::vector<std::string>& types)
{
    if (types.empty()) return true;

    std::string ext = p.extension().string();
    for (auto& c : ext) c = tolower(c);

    for (const auto& t : types) {
        if (ext == t) return true;
    }
    return false;
}

bool isRiskyExtension(const fs::path& p)
{
    static const std::vector<std::string> risky = {
        ".exe", ".bat", ".cmd", ".com", ".ps1"
    };

    std::string ext = p.extension().string();
    for (auto& c : ext) c = tolower(c);

    for (const auto& r : risky) {
        if (ext == r) return true;
    }
    return false;
}

void printHelp()
{
    std::cout <<
        "Usage:\n"
        "  random_open <folder_path> [--type <ext1,ext2,...>]\n\n"
        "Options:\n"
        "  --type <list>       Filter by file extensions (comma-separated)\n"
        "  -h, --help, /h, /?  Show this help message\n\n"
        "Examples:\n"
        "  random_open C:\\Images --type png,jpg,webp\n"
        "  random_open C:\\Music --type=.mp3,.wav\n";
}

int main(int argc, char* argv[])
{
    if (argc == 1) {
        printHelp();
        return 1;
    }

    std::string folderArg;
    std::vector<std::string> typeFilters;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help" || arg == "/h" || arg == "/?") {
            printHelp();
            return 0;
        }
        else if (arg == "--type") {
            if (i + 1 >= argc) {
                printHelp();
                return 1;
            }
            auto exts = splitExtensions(argv[++i]);
            typeFilters.insert(typeFilters.end(), exts.begin(), exts.end());
        }
        else if (arg.rfind("--type=", 0) == 0) {
            auto exts = splitExtensions(arg.substr(7));
            typeFilters.insert(typeFilters.end(), exts.begin(), exts.end());
        }
        else if (folderArg.empty()) {
            folderArg = arg;
        }
        else {
            printHelp();
            return 1;
        }
    }

    if (folderArg.empty()) {
        printHelp();
        return 1;
    }

    fs::path folder = folderArg;

    if (!fs::exists(folder) || !fs::is_directory(folder)) {
        std::cout << "The specified folder does not exist or is invalid.\n"; // If the folder is invalid or does not exist
        return 1;
    }

    std::vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (!entry.is_regular_file()) continue;
        if (!matchesType(entry.path(), typeFilters)) continue;
        files.push_back(entry.path());
    }

    if (files.empty()) {
        std::cout << "There are no files in the specified folder.\n"; // If there is no file to open
        return 1;
    }

    char selfPath[MAX_PATH];
    GetModuleFileNameA(nullptr, selfPath, MAX_PATH);
    fs::path self = fs::canonical(selfPath);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, files.size() - 1);

    fs::path selected = fs::canonical(files[dist(gen)]);

    // Exit silently if this executable is selected
    if (selected == self) {
        return 0;
    }

    if (isRiskyExtension(selected)) {
        std::cout << "This file type may be dangerous. Open it? (Y/N): ";
        char answer;
        std::cin >> answer;
        if (answer != 'Y' && answer != 'y') {
            return 0;
        }
    }

    ShellExecuteA(
        nullptr,
        "open",
        selected.string().c_str(),
        nullptr,
        nullptr,
        SW_SHOWNORMAL
    );

    return 0;
}