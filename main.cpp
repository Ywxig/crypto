#include <iostream>
#include <string>
#include <cstdlib>

#include "include/colorise.hpp"
#include "include/algoritms.hpp"


int main(int argc, char *argv[]) {
    if (argc < 2) {

        std::cout << echo("");
        exit(1);
    }

    std::string opt = argv[1];

    // Options that do not require a third argument (message)
    if (opt == "-test" || opt == "-v") {
        if (opt == "-test") {
            [[maybe_unused]] uint8_t a = 0x57;
            [[maybe_unused]] uint8_t b = 0x83;
            // Let's call gf_mult if available or handle it
            uint8_t res = gf_mult(a, b);
            std::cout << "0x" << std::hex << (int)a
                      << " * 0x" << (int)b
                      << " = 0x" << (int)res << std::dec << std::endl;
            std::cout << "Test option executed successfully.\n";
            exit(0);
        } else if (opt == "-v") {
            std::cout << "Version 1.0.0\n";
            exit(0);
        }
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

    std::string msg = argv[2];


    // Реализация поля галуа
    if (opt == "-Xmp") {
        gf_mult(argv[2][0], argv[3][0]);
        exit(0);
    } else if (opt == "-Eae") {
        poly_ext_gcd(argv[2][0]);
        std::cout << argv[2][0] << " " << std::endl;
        exit(0);
    } else if (opt == "-Tlog") {
        ;
    } else {
        std::cerr << echo("<red> " + (std::string)argv[0] + ": unknown option '" + opt + "'\n");
        exit(3);
    }

    return 0;
}
