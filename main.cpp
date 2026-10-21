#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdint>

#include "include/globals.h"
#include "include/file.h"
#include "include/colorise.hpp"
#include "include/math_x.h"

#include "include/test_math.h"

const char LETTERS[] = "abcdefghijklmnopqrstuvwxyz";
const char LETTERS2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char LETTERS3[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";

static void print_usage(const char *prog) {
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
        "Usage: <green> " + std::string(prog) + " <magenta> <option> <reset> <message>\n"
        "Options:\n"
        " <yellow> -Ecsr <reset> Encrypt (Caesar Shift)\n"
        " <yellow> -Dcsr <reset> Decrypt (Caesar Shift)\n"
        " <yellow> -Ecsk <reset> Encrypt (Caesar Keyword)\n"
        " <yellow> -Dcsk <reset> Decrypt (Caesar Keyword)\n"
        " <yellow> -Evgn <reset> Encrypt (Vigenere)\n"
        " <yellow> -Dvgn <reset> Decrypt (Vigenere)\n"
        " <yellow> -Erc2 <reset> Encrypt (RC2): -Erc2 <message>\n"
        " <yellow> -Drc2 <reset> Decrypt (RC2): -Drc2 <key> <cipher_hex>\n"
        " <yellow> -GFm <reset> GF(2) multiplication\n"
        " <yellow> -PEG <reset> Polynomial Extended GCD\n"
        " <yellow> -Tlog <reset> GF(2) multiplication table\n"
        " <yellow> -F <reset> Read file\n"
        " <yellow> -I <reset> Interactive mode\n"
        " <yellow> -v <reset> Version info\n");
}

// ---------------------------------------------------------------------------
// RC2
// ---------------------------------------------------------------------------
static int rc2_encrypt_cmd(const std::string &text) {
    long long key = DIFFI_HELLMAN();

    std::vector<uint8_t> out(text.size() + 8);
    int n = RC2_encrypt(text.c_str(), key, out.data(), out.size());
    if (n < 0) {
        std::cerr << "RC2: encryption failed\n";
        return 4;
    }

    std::cout << "Key: " << key << "\n";
    std::cout << "Cipher: ";
    for (int i = 0; i < n; i++) {
        std::printf("%02X", out[i]);
    }
    std::cout << "\n";
    return 0;
}

static int rc2_decrypt_cmd(const char *prog, const char *key_str, const char *hex_str) {
    long long key = 0;
    try {
        key = std::stoll(key_str);
    } catch (...) {
        std::cerr << prog << ": invalid key '" << key_str << "'\n";
        return 2;
    }

    std::string hex = hex_str;
    std::vector<uint8_t> cipher(hex.size() / 2 + 1);
    int n = hex_to_bytes(hex.c_str(), cipher.data(), cipher.size());
    if (n <= 0 || n % 8 != 0) {
        std::cerr << prog << ": cipher must be hex, length multiple of 16 chars\n";
        return 2;
    }

    std::vector<char> plain(n + 1);
    int m = RC2_decrypt(cipher.data(), (size_t)n, key, plain.data(), plain.size());
    if (m < 0) {
        std::cerr << prog << ": decryption failed (wrong key or corrupted data)\n";
        return 4;
    }

    std::cout << "Decrypted: " << std::string(plain.data(), m) << "\n";
    return 0;
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        exit(1);
    }

    std::string opt = argv[1];
    std::string msg_str = "";
    char *msg = nullptr;
    std::string alg = "";
    std::string file = "";

    // Интерактивный режим (-I)
    if (opt == "-I") {
        if (argc > 2) {
            alg = argv[2];
        } else {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": missing algorithm argument for -I\n");
            exit(2);
        }
    }
    // Тесты
    else if (opt == "-t" || opt == "-T") {
        START_TEST();
        exit(0);
    }
    // Режим файлов (-F)
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
        if (argc < 4) {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": -GFm needs two arguments\n");
            exit(2);
        }
        gf_mult(argv[2][0], argv[3][0]);
        exit(0);
    } else if (opt == "-dfh") {
        DIFFI_HELLMAN();
    } else if (opt == "-Erc2") {
        // ./prog -Erc2 "message"
        return rc2_encrypt_cmd(msg_str);
    } else if (opt == "-Drc2") {
        // ./prog -Drc2 <key> <cipher_hex>
        if (argc < 4) {
            std::cerr << "Usage: " << argv[0] << " -Drc2 <key> <cipher_hex>\n";
            exit(2);
        }
        return rc2_decrypt_cmd(argv[0], argv[2], argv[3]);
    } else if (opt == "-Eidea") {
        long long key = DIFFI_HELLMAN();
        IDEA_encrypt(msg, (char *)&key);
    } else if (opt == "-I") {
        if (alg == "Idh") {
            DIFFI_HELLMAN();
        } else {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": unknown interactive algorithm '" + alg + "'\n");
            exit(3);
        }
    } else if (opt == "-F") {
        // fileRead возвращает временный объект — сохраняем в переменную
        auto data = fileRead(file);

        if (alg == "Ecsr") {
            CSR_encrypt(data.data());
        } else if (alg == "Dcsr") {
            CSR_decrypt(data.data());
        } else if (alg == "Ecsk") {
            encrypt_caesar_keyword(data.data());
        } else if (alg == "Dcsk") {
            decrypt_caesar_keyword(data.data());
        } else if (alg == "Evgn") {
            VGN_encript(data.data());
        } else if (alg == "Dvgn") {
            VGN_decript(data.data());
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
