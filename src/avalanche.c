#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../include/globals.h"

/*
 * Измерение лавинного эффекта: меняем один бит ключа и считаем,
 * сколько бит шифротекста изменилось. В идеале — около 50%.
 */

static int count_diff_bits(const uint8_t *a, const uint8_t *b, size_t len) {
    int bits = 0;

    for (size_t i = 0; i < len; i++) {
        uint8_t x = (uint8_t)(a[i] ^ b[i]);

        while (x) {
            bits += x & 1;
            x >>= 1;
        }
    }

    return bits;
}

static void print_result(const char *name, long long changed, long long total) {
    printf("  %-12s %6lld / %6lld бит  (%.1f%%)\n",
           name, changed, total, total ? 100.0 * (double)changed / (double)total : 0.0);
}

void avalanche_test(void) {
    const uint8_t block[8] = {'A', 'V', 'A', 'L', 'N', 'C', 'H', '!'};
    const char *text = "AVALNCH!";
    long long changed;
    long long total;

    printf("Лавинный эффект (один бит ключа -> шифротекст)\n");

    /* IDEA */
    {
        uint8_t key[16] = {0};
        uint8_t base[8], test[8];
        uint16_t Z[52];

        IDEA_keyexp(key, Z);
        memcpy(base, block, 8);
        IDEA_encrypt(Z, base);

        changed = 0;
        for (int bit = 0; bit < 128; bit++) {
            uint8_t k[16];

            memcpy(k, key, 16);
            k[bit / 8] ^= (uint8_t)(1u << (bit % 8));

            IDEA_keyexp(k, Z);
            memcpy(test, block, 8);
            IDEA_encrypt(Z, test);

            changed += count_diff_bits(base, test, 8);
        }
        total = 128LL * 64LL;
        print_result("IDEA", changed, total);
    }

    /* ГОСТ 28147-89 */
    {
        uint8_t key[32] = {0};
        uint8_t base[8], test[8];
        uint32_t K[8];

        GOST_keyexp(key, K);
        memcpy(base, block, 8);
        GOST_encrypt(K, base);

        changed = 0;
        for (int bit = 0; bit < 256; bit++) {
            uint8_t k[32];

            memcpy(k, key, 32);
            k[bit / 8] ^= (uint8_t)(1u << (bit % 8));

            GOST_keyexp(k, K);
            memcpy(test, block, 8);
            GOST_encrypt(K, test);

            changed += count_diff_bits(base, test, 8);
        }
        total = 256LL * 64LL;
        print_result("GOST", changed, total);
    }

    /* RC2 (из лабораторной 02) */
    {
        long long key = 0x0123456789ABCDEFLL;
        uint8_t base[16], test[16];

        RC2_encrypt(text, key, base, sizeof(base));

        changed = 0;
        for (int bit = 0; bit < 64; bit++) {
            long long k = key ^ (long long)(1ULL << bit);

            RC2_encrypt(text, k, test, sizeof(test));
            changed += count_diff_bits(base, test, 16);
        }
        total = 64LL * 128LL;
        print_result("RC2", changed, total);
    }
}
