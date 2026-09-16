#include <iostream>
#include <string>

#include "include/globals.h"
#include "include/file.h"
#include "include/colorise.hpp"

char LETTERS[] = "abcdefghijklmnopqrstuvwxyz";

int main(int argc, char *argv[]) {
    if (argc < 2) {

        std::cout << echo("Usage: " + std::string(argv[0]) + " <option> <message>\n"
             "Options:\n"
             " <yellow> -Ecsr <reset> Encrypt (Caesar Shift)\n"
             " <yellow> -Dcsr <reset> Decrypt (Caesar Shift)\n"
             " <yellow> -Ecsk <reset> Encrypt (Caesar Keyword)\n"
             " <yellow> -Dcsk <reset> Decrypt (Caesar Keyword)\n"
             " <yellow> -Evgn <reset> Encrypt (Vigenere)\n"
             " <yellow> -Dvgn <reset> Decrypt (Vigenere)\n"
             " <yellow> -F <reset> Read file\n");
        exit(1);
    }

    std::string opt = argv[1];

    if (opt == "-v") {
        std::cout << "v1.0.0\n";
        exit(0);
    }

    if (argc < 3) {
        std::cerr << argv[0] << ": missing message argument\n";
        exit(2);
    }

    // Для C-функций, принимающих char*, берем .data() или c_str()
    // Обратите внимание: если функции модифицируют строку на месте, нужно передавать неконстантный указатель.
    // Используем константную копию или преобразуем в неконстантный чар, если функции Си мутируют строку.
    std::string msg_str = argv[2];
    char *msg = msg_str.data();

    if (opt == "-Ecsr") {
        encrypt_caesar_shift(msg);
    } else if (opt == "-Dcsr") {
        decrypt_caesar_shift(msg);
    } else if (opt == "-Ecsk") {
        encrypt_caesar_keyword(msg);
    } else if (opt == "-Dcsk") {
        decrypt_caesar_keyword(msg);
    } else if (opt == "-Evgn") {
        VGN_encript(msg);
    } else if (opt == "-Dvgn") {
        VGN_decript(msg);
    } else if (opt == "-F") {
        std::string content = fileRead(argv[2]);
        std::cout << content << "\n";
    } else {
        std::cerr << argv[0] << ": unknown option '" << opt << "'\n";
        exit(3);
    }

    return 0;
}
