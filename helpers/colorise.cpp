// Твой основной код на C++ (оставляем как есть, только убираем дублирование)

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "../include/utils.hpp"
#include <cstdarg>

// хэш-таблица для хранения цветовых кодов ANSI
inline const std::unordered_map<std::string, std::string>& getColors() {
    static const std::unordered_map<std::string, std::string> COLORS = {
        {"<red>", "\033[31m"},
        {"<green>", "\033[32m"},
        {"<yellow>", "\033[33m"},
        {"<blue>", "\033[34m"},
        {"<magenta>", "\033[35m"},
        {"<cyan>", "\033[36m"},
        {"<white>", "\033[37m"},
        {"<reset>", "\033[0m"},
        {"<enc>", "\033[33m Encrypt: \033[0m"},
        {"<dec>", "\033[33m Decrypt: \033[0m"},
        {"<key>", "\033[33m Key: \033[0m"}
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
    std::vector<std::string> tokens = split(message.c_str());

    for (size_t i = 0; i < tokens.size(); i++) {
        auto it = COLORS.find(tokens[i]);
        if (it != COLORS.end()) {
            result += it->second;
        } else {
            result += tokens[i];
        }
        if (i + 1 < tokens.size()) {
            result += " ";
        }
    }
    result += "\033[0m";
    return result;
}

// МОСТ ДЛЯ C-КОДА (C-Compatible Wrapper)

extern "C" {
    // Безопасный аналог printf с поддержкой ваших тегов
    void c_print_format(const char* format, ...) {
        if (!format) return;

        va_list args;
        va_start(args, format);
        va_list args_copy;
        va_copy(args_copy, args);
        int len = std::vsnprintf(nullptr, 0, format, args_copy);
        va_end(args_copy);

        std::string formatted_msg(len, '\0');
        std::vsnprintf(&formatted_msg[0], len + 1, format, args);
        va_end(args);

        // Передаем готовый текст в ваш существующий метод echo
        std::string final_output = echo(formatted_msg);
        std::cout << final_output << std::endl;
    }
}

// extern "C" {
//     // Функция для вызова из C-алгоритма.
//     // Принимает const char*, сама вызывает твой C++ echo и печатает результат в stdout.
//     void c_print_colored(const char* message) {
//         if (!message) return;
//         // Вызываем твою C++ функцию echo
//         std::string formatted = echo(std::string(message));
//         // Выводим через C++ поток
//         std::cout << formatted << std::endl;
//     }
// }
