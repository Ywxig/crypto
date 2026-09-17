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
    std::string msg_str = "";
    char *msg = nullptr;
    std::string alg = "";
    std::string file = "";
    if (argc > 3) {
        alg = argv[2];
        file = argv[3];
    } else if (argc > 2) {
        // Для C-функций, принимающих char*, берем .data() или c_str()
        // Обратите внимание: если функции модифицируют строку на месте, нужно передавать неконстантный указатель.
        // Используем константную копию или преобразуем в неконстантный чар, если функции Си мутируют строку.
        msg_str = argv[2];
        msg = msg_str.data();
    }

    if (opt == "-v") {
        std::cout << "v1.0.0\n";
        exit(0);
    }

    if (argc < 3) {
        std::cerr << argv[0] << ": missing message argument\n";
        exit(2);
    }


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

        if (alg == "Ecsr") {
            encrypt_caesar_shift(fileRead(file).data());
        } else if (alg == "Dcsr") {
            decrypt_caesar_shift(fileRead(file).data());
        } else if (alg == "Ecsk") {
            encrypt_caesar_keyword(fileRead(file).data());
        } else if (alg == "Dcsk") {
            decrypt_caesar_keyword(fileRead(file).data());
        } else if (alg == "Evgn") {
            VGN_encript(fileRead(file).data());
        } else if (alg == "Dvgn") {
            VGN_decript(fileRead(file).data());
        } else {
            std::cerr << argv[0] << ": unknown algorithm '" << alg << "'\n";
            exit(3);
        }

    } else {
        std::cerr << argv[0] << ": unknown option '" << opt << "'\n";
        exit(3);
    }

    return 0;
}
