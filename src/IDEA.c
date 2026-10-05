#include "../include/math_x.h"
#include <string.h>

void IDEA_encrypt(char* msg, char* key) {
    uint16_t A, B, C, D;
    memcpy(&A, msg + 0, 2);
    memcpy(&B, msg + 2, 2);
    memcpy(&C, msg + 4, 2);
    memcpy(&D, msg + 6, 2);



}
