/*
 * Общие алфавиты, используемые шифрами Цезаря и Виженера.
 *
 * Определения вынесены в отдельный файл, чтобы динамическая библиотека
 * libcrypto.so была самодостаточной. Символы помечены как weak: если
 * приложение определяет эти же массивы само (как main.cpp), его
 * определения имеют приоритет и конфликта при линковке не возникает.
 */

#include "../include/globals.h"

__attribute__((weak)) const char LETTERS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
__attribute__((weak)) const char LETTERS2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
__attribute__((weak)) const char LETTERS3[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
