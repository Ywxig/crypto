// тут реализация раскраски командной строки для красивого вывода в терминале

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "utils.hpp"

// Функция разделения строки (если она у вас объявлена где-то в utils.hpp / utils.h)
// Убедитесь, что подключен правильный заголовок
// #include "utils.hpp"

// Инициализация цветов с помощью лямбды или внутри функции,
// так как прямое присваивание вне функций в C++ недопустимо.
inline const std::unordered_map<std::string, std::string>& getColors() {
    static const std::unordered_map<std::string, std::string> COLORS = {
        {"red", "\033[31m"},
        {"green", "\033[32m"},
        {"yellow", "\033[33m"},
        {"blue", "\033[34m"},
        {"magenta", "\033[35m"},
        {"cyan", "\033[36m"},
        {"white", "\033[37m"},
        {"reset", "\033[0m"}
    };
    return COLORS;
}

void color(const std::string& colorCode) {
    std::cout << colorCode;
}

void resetColor() {
    std::cout << "\033[0m" << std::endl;
}

std::string echo(const std::string& message) {
    std::string result = "";
    const auto& COLORS = getColors();

    // Предполагаем, что функция split доступна (возвращает std::vector<std::string>)
    std::vector<std::string> tokens = split(message.c_str());

    for (size_t i = 0; i < tokens.size(); i++) {
        auto it = COLORS.find(tokens[i]);
        if (it != COLORS.end()) {
            // Если токен найден в цветах, добавляем ANSI-код
            result += it->second;
        } else {
            // Иначе оставляем текст как есть
            result += tokens[i];
        }

        // Добавляем пробел между токенами, если это не последний токен
        if (i + 1 < tokens.size()) {
            result += " ";
        }
    }

    // Автоматически сбрасываем цвет в конце, чтобы не ломать терминал
    result += "\033[0m";

    return result;
}
