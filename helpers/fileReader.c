#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* fileRead(const char* filename) {
    FILE* fileptr = fopen(filename, "r");
    if (fileptr == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL; // Или exit(6), если это критично для вашей программы
    }

    // Переходим в конец файла, чтобы узнать его размер
    if (fseek(fileptr, 0, SEEK_END) != 0) {
        fclose(fileptr);
        return NULL;
    }

    long length = ftell(fileptr);
    if (length < 0) {
        fclose(fileptr);
        return NULL;
    }

    // Возвращаемся в начало файла для чтения
    rewind(fileptr);

    // Выделяем память под весь файл + 1 байт для нуль-терминатора
    char* buffer = (char*)malloc(length + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(fileptr);
        return NULL;
    }

    // Читаем файл в буфер
    size_t read_bytes = fread(buffer, 1, length, fileptr);
    buffer[read_bytes] = '\0'; // Гарантируем завершение строки

    fclose(fileptr);
    return buffer;
}
