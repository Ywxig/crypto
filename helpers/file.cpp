// это файлик где реализована работа с файлами

#include <fstream>
#include <string>

std::string fileRead(const std::string& filename) {
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}
