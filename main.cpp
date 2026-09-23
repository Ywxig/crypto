#include <iostream>
#include <string>

#include "include/globals.h"
#include "include/file.h"
#include "include/colorise.hpp"
#include "include/math_x.h"


const char LETTERS[] = "abcdefghijklmnopqrstuvwxyz";
const char LETTERS2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char LETTERS3[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";

int main(int argc, char *argv[]) {
    if (argc < 2) {

        std::cout << echo(
            "* <magenta> Copyright (C) 2026 Dmitrii Zimnov (Ywxig) <reset> \n"
            "*\n"
            "* This program is free software: you can redistribute it and/or modify\n"
            "* it under the terms of the GNU General Public License as published by\n"
            "* the Free Software Foundation, either version 3 of the License, or\n"
            "* (at your option) any later version.\n"
            "*\n"
            "* This program is distributed in the hope that it will be useful,\n"
            "* but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
            "* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
            "* GNU General Public License for more details.\n"
            "*\n"
            "* You should have received a copy of the GNU General Public License\n"
            "* along with this program.  If not, see <https://www.gnu.org/licenses/>.\n"
            "\n"
            "\n"
            "\n"
            "Usage: <green> " + std::string(argv[0]) + " <magenta> <option> <reset> <message>\n"
             "Options:\n"
             " <yellow> -Ecsr <reset> Encrypt (Caesar Shift)\n"
             " <yellow> -Dcsr <reset> Decrypt (Caesar Shift)\n"
             " <yellow> -Ecsk <reset> Encrypt (Caesar Keyword)\n"
             " <yellow> -Dcsk <reset> Decrypt (Caesar Keyword)\n"
             " <yellow> -Evgn <reset> Encrypt (Vigenere)\n"
             " <yellow> -Dvgn <reset> Decrypt (Vigenere)\n"
             " <yellow> -GFm <reset> GF(2) multiplication\n"
             " <yellow> -PEG <reset> Polynomial Extended GCD\n"
             " <yellow> -Tlog <reset> GF(2) multiplication table\n"
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
            std::cerr << echo("<red> " + (std::string)argv[0] + ": missing algorithm argument for -I\n");
            exit(2);
        }
    }
    // Обработка режима файлов (-F)
    else if (opt == "-F") {
        if (argc > 3) {
            alg = argv[2];
            file = argv[3];
        } else {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": missing algorithm or file argument for -F\n");
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
            if (opt[0] == '-') {
                std::cerr << echo("<red> " + (std::string)argv[0] + ": missing message argument\n");
                exit(2);
            } else {
                std::cerr << echo("<red> " + (std::string)argv[0] + ": missing option argument\n");
                exit(2);
            }
        }
        msg_str = argv[2];
        msg = msg_str.data();
    }


    if (opt == "-Ecsr") {
        CSR_encrypt(msg);
    } else if (opt == "-Dcsr") {
        CSR_decrypt(msg);
    } else if (opt == "-Dcsrm") {
        CSR_dec_math(msg);
    } else if (opt == "-Ecsm") {
        CSR_enc_math(msg);
    } else if (opt == "-Ecsk") {
        encrypt_caesar_keyword(msg);
    } else if (opt == "-Dcsk") {
        decrypt_caesar_keyword(msg);
    } else if (opt == "-Evgn") {
        VGN_encript(msg);
    } else if (opt == "-Dvgn") {
        VGN_decript(msg);
    } else if (opt == "-Evgns") {
        VGN_enc_seed(msg);
    } else if (opt == "-GFm") {
        gf_mult(argv[2][0], argv[3][0]);
        exit(0);
    } else if (opt == "-PEG") {
        poly_ext_gcd(argv[2][0], 0x11B);
        std::cout << argv[2][0] << " " << std::endl;
        exit(0);
    } else if (opt == "-Tlog") {
        gf_mult_table(argv[2][0], argv[3][0]);
        exit(0);
    } else if (opt == "-I") {
        if (alg == "Idh") {
            DIFFI_HELLMAN();
        } else {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": unknown interactive algorithm '" + alg + "'\n");
            exit(3);
        }
    } else if (opt == "-F") {

        if (alg == "Ecsr") {
            CSR_encrypt(fileRead(file).data());
        } else if (alg == "Dcsr") {
            CSR_decrypt(fileRead(file).data());
        } else if (alg == "Ecsk") {
            encrypt_caesar_keyword(fileRead(file).data());
        } else if (alg == "Dcsk") {
            decrypt_caesar_keyword(fileRead(file).data());
        } else if (alg == "Evgn") {
            VGN_encript(fileRead(file).data());
        } else if (alg == "Dvgn") {
            VGN_decript(fileRead(file).data());
        } else {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": unknown algorithm '" + alg + "'\n");
            exit(3);
        }

    } else {
        std::cerr << echo("<red> " + (std::string)argv[0] + ": unknown option '" + opt + "'\n");
        exit(3);
    }

    return 0;
}
