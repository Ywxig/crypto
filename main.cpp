#include <iostream>
#include <string>
#include <cstring>

#include "include/globals.h"
#include "include/file.h"
#include "include/colorise.hpp"
#include "include/math_x.h"

#include "include/test_math.h"

const char LETTERS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char LETTERS2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char LETTERS3[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";

// Разбор hex-ключа заданной длины
static int parse_hex_key(const char *s, uint8_t *key, size_t len) {
    if (!s || strlen(s) != len * 2) return -1;
    return hex_to_bytes(s, key, len) == (int)len ? 0 : -1;
}

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
        " <yellow> -Eidea <reset> Encrypt (IDEA): -Eidea <message> [key_hex]\n"
        " <yellow> -Didea <reset> Decrypt (IDEA): -Didea [key_hex] <cipher_hex>\n"
        " <yellow> -Egost <reset> Encrypt (GOST): -Egost <message> [key_hex]\n"
        " <yellow> -Dgost <reset> Decrypt (GOST): -Dgost [key_hex] <cipher_hex>\n"
        " <yellow> -Eidea-ctr <reset> IDEA CTR: -Eidea-ctr <message> [key_hex]\n"
        " <yellow> -Didea-ctr <reset> IDEA CTR: -Didea-ctr <iv_hex> <cipher_hex> [key_hex]\n"
        " <yellow> -Egost-ctr <reset> GOST CTR: -Egost-ctr <message> [key_hex]\n"
        " <yellow> -Dgost-ctr <reset> GOST CTR: -Dgost-ctr <iv_hex> <cipher_hex> [key_hex]\n"
        " <yellow> -Mgost <reset> GOST MAC: -Mgost <message> [key_hex]\n"
        " <yellow> -Ekuzy <reset> Encrypt (Kuznyechik): -Ekuzy <message> [key_hex]\n"
        " <yellow> -Dkuzy <reset> Decrypt (Kuznyechik): -Dkuzy [key_hex] <cipher_hex>\n"
        " <yellow> -avalanche <reset> Avalanche effect (IDEA/GOST/RC2)\n"
        " <yellow> -GFm <reset> GF(2) multiplication\n"
        " <yellow> -PEG <reset> Polynomial Extended GCD\n"
        " <yellow> -Tlog <reset> GF(2) multiplication table\n"
        " <yellow> -F <reset> Read file\n"
        " <yellow> -I <reset> Interactive mode\n"
        " <yellow> -v <reset> Version info\n");
}


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
        if (opt == "-avalanche") {
            avalanche_test();
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
    }  else if (opt == "-GFm") {
        if (argc < 4) {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": -GFm needs two arguments\n");
            exit(2);
        }
        gf_mult(argv[2][0], argv[3][0]);
        exit(0);
    } else if (opt == "-dfh") {
        DIFFI_HELLMAN();
    } else if (opt == "-dfhc") {
        DH_ELP_CURV();
    } else if (opt == "-Erc2") {
        // ./prog -Erc2 "message"
        return rc2_encrypt_cmd(msg_str.c_str());
    } else if (opt == "-Drc2") {
        // ./prog -Drc2 <key> <cipher_hex>
        if (argc < 4) {
            std::cerr << "Usage: " << argv[0] << " -Drc2 <key> <cipher_hex>\n";
            exit(2);
        }
        return rc2_decrypt_cmd(argv[0], argv[2], argv[3]);
    } else if (opt == "-Eidea") {
        uint8_t key[16];
        memcpy(key, idea_default_key, sizeof(key));
        if (argc >= 4 && parse_hex_key(argv[3], key, sizeof(key)) != 0) {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": IDEA key must be 32 hex chars\n");
            exit(2);
        }
        return IDEA_encrypt_cmd(msg, key);
    } else if (opt == "-Didea") {
        uint8_t key[16];
        const char *cipher_hex;
        memcpy(key, idea_default_key, sizeof(key));
        if (argc >= 4) {
            if (parse_hex_key(argv[2], key, sizeof(key)) != 0) {
                std::cerr << echo("<red> " + (std::string)argv[0] + ": IDEA key must be 32 hex chars\n");
                exit(2);
            }
            cipher_hex = argv[3];
        } else {
            cipher_hex = argv[2];
        }
        return IDEA_decrypt_cmd(argv[0], key, cipher_hex);
    } else if (opt == "-Egost") {
        uint8_t key[32];
        memcpy(key, gost_default_key, sizeof(key));
        if (argc >= 4 && parse_hex_key(argv[3], key, sizeof(key)) != 0) {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": GOST key must be 64 hex chars\n");
            exit(2);
        }
        return GOST_encrypt_cmd(msg, key);
    } else if (opt == "-Dgost") {
        uint8_t key[32];
        const char *cipher_hex;
        memcpy(key, gost_default_key, sizeof(key));
        if (argc >= 4) {
            if (parse_hex_key(argv[2], key, sizeof(key)) != 0) {
                std::cerr << echo("<red> " + (std::string)argv[0] + ": GOST key must be 64 hex chars\n");
                exit(2);
            }
            cipher_hex = argv[3];
        } else {
            cipher_hex = argv[2];
        }
        return GOST_decrypt_cmd(argv[0], key, cipher_hex);
    } else if (opt == "-Eidea-ctr") {
        uint8_t key[16];
        memcpy(key, idea_default_key, sizeof(key));
        if (argc >= 4 && parse_hex_key(argv[3], key, sizeof(key)) != 0) {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": IDEA key must be 32 hex chars\n");
            exit(2);
        }
        return IDEA_ctr_encrypt_cmd(msg, key);
    } else if (opt == "-Didea-ctr") {
        uint8_t key[16];
        memcpy(key, idea_default_key, sizeof(key));
        if (argc < 4) {
            std::cerr << "Usage: " << argv[0] << " -Didea-ctr <iv_hex> <cipher_hex> [key_hex]\n";
            exit(2);
        }
        if (argc >= 5 && parse_hex_key(argv[4], key, sizeof(key)) != 0) {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": IDEA key must be 32 hex chars\n");
            exit(2);
        }
        return IDEA_ctr_decrypt_cmd(argv[0], key, argv[2], argv[3]);
    } else if (opt == "-Egost-ctr") {
        uint8_t key[32];
        memcpy(key, gost_default_key, sizeof(key));
        if (argc >= 4 && parse_hex_key(argv[3], key, sizeof(key)) != 0) {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": GOST key must be 64 hex chars\n");
            exit(2);
        }
        return GOST_ctr_encrypt_cmd(msg, key);
    } else if (opt == "-Dgost-ctr") {
        uint8_t key[32];
        memcpy(key, gost_default_key, sizeof(key));
        if (argc < 4) {
            std::cerr << "Usage: " << argv[0] << " -Dgost-ctr <iv_hex> <cipher_hex> [key_hex]\n";
            exit(2);
        }
        if (argc >= 5 && parse_hex_key(argv[4], key, sizeof(key)) != 0) {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": GOST key must be 64 hex chars\n");
            exit(2);
        }
        return GOST_ctr_decrypt_cmd(argv[0], key, argv[2], argv[3]);
    } else if (opt == "-Mgost") {
        uint8_t key[32];
        memcpy(key, gost_default_key, sizeof(key));
        if (argc >= 4 && parse_hex_key(argv[3], key, sizeof(key)) != 0) {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": GOST key must be 64 hex chars\n");
            exit(2);
        }
        return GOST_mac_cmd(msg, key);
    } else if (opt == "-Ekuzy") {
        uint8_t key[32];
        memcpy(key, kuznyechik_default_key, sizeof(key));
        if (argc >= 4 && parse_hex_key(argv[3], key, sizeof(key)) != 0) {
            std::cerr << echo("<red> " + (std::string)argv[0] + ": Kuznyechik key must be 64 hex chars\n");
            exit(2);
        }
        return kuznyechik_encrypt_cmd(msg, key);
    } else if (opt == "-Dkuzy") {
        uint8_t key[32];
        const char *cipher_hex;
        memcpy(key, kuznyechik_default_key, sizeof(key));
        if (argc >= 4) {
            if (parse_hex_key(argv[2], key, sizeof(key)) != 0) {
                std::cerr << echo("<red> " + (std::string)argv[0] + ": Kuznyechik key must be 64 hex chars\n");
                exit(2);
            }
            cipher_hex = argv[3];
        } else {
            cipher_hex = argv[2];
        }
        return kuznyechik_decrypt_cmd(argv[0], key, cipher_hex);
    }  else if (opt == "-I") {
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
