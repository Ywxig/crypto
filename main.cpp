#include <iostream>
#include <string>

#include "include/globals.h"
#include "include/file.h"
#include "include/colorise.hpp"

char LETTERS[] = "abcdefghijklmnopqrstuvwxyz";

int main(int argc, char *argv[]) {
    if (argc < 2) {

        std::cout << echo("Usage: <green> " + std::string(argv[0]) + " <magenta> <option> <reset> <message>\n"
             "Options:\n"
             " <yellow> -Ecsr <reset> Encrypt (Caesar Shift)\n"
             " <yellow> -Dcsr <reset> Decrypt (Caesar Shift)\n"
             " <yellow> -Ecsk <reset> Encrypt (Caesar Keyword)\n"
             " <yellow> -Dcsk <reset> Decrypt (Caesar Keyword)\n"
             " <yellow> -Evgn <reset> Encrypt (Vigenere)\n"
             " <yellow> -Dvgn <reset> Decrypt (Vigenere)\n"
             " <yellow> -F <reset> Read file\n"
             " <yellow> -I <reset> Interactive mode\n"
             " <yellow> -v <reset> Version info\n");
        exit(1);
    }

    std::string opt = argv[1];
    std::string msg_str = "";
    char *msg = nullptr;
    std::string alg = "";
    std::string file = "";

    // Обработка интерактивного режима (-I)
    if (opt == "-I") {
        if (argc > 2) {
            alg = argv[2];
        } else {
            std::cerr << argv[0] << ": missing algorithm argument for -I\n";
            exit(2);
        }
    }
    // Обработка режима файлов (-F)
    else if (opt == "-F") {
        if (argc > 3) {
            alg = argv[2];
            file = argv[3];
        } else {
            std::cerr << argv[0] << ": missing algorithm or file argument for -F\n";
            exit(2);
        }
    }
    // Обычный режим с сообщением
    else {
        if (opt == "-v") {
            std::cout << "v1.0.0\n";
            exit(0);
        }
        if (argc < 3) {
            std::cerr << argv[0] << ": missing message argument\n";
            exit(2);
        }
        msg_str = argv[2];
        msg = msg_str.data();
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

    } else if (opt == "-I") {
        if (alg == "Idh") {
            DIFFI_HELLMAN();
        } else {
            std::cerr << argv[0] << ": unknown interactive algorithm '" << alg << "'\n";
            exit(3);
        }
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
