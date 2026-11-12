#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../include/globals.h"
#include "../include/colorise.h"
#include "../include/test_math.h"

static int hex_equal(const uint8_t *buf, size_t len, const char *hex) {
    uint8_t tmp[256];
    int n = hex_to_bytes(hex, tmp, sizeof(tmp));

    return n == (int)len && memcmp(buf, tmp, len) == 0;
}

static void test_idea_vector(void) {
    uint8_t key[16];
    uint8_t cipher[64];
    char plain[64];
    const char *msg = "IWILLBEHEREONMONDAY";
    int n, m;

    c_print_format(" <green> [1/5] <reset> IDEA: тестовый вектор...\n");

    assert(hex_to_bytes("00010002000300040005000600070008", key, sizeof(key)) == 16);

    n = idea_encrypt_msg(key, msg, cipher, sizeof(cipher));
    assert(n == 24);
    assert(hex_equal(cipher, (size_t)n,
                     "1D3B834B322E6E03294573978AD57F71AA69D3E8B9132591"));

    m = idea_decrypt_msg(key, cipher, (size_t)n, plain, sizeof(plain));
    assert(m == (int)strlen(msg));
    assert(strcmp(plain, msg) == 0);

    c_print_format(" <green> -> IDEA: УСПЕШНО\n\n");
}

static void test_gost_vector(void) {
    uint8_t key[32];
    uint8_t cipher[64];
    char plain[64];
    const char *msg = "IWILLBEHEREONMONDAY";
    int n, m;

    c_print_format(" <green> [2/5] <reset> ГОСТ 28147-89: тестовый вектор...\n");

    assert(hex_to_bytes("ffeeddccbbaa99887766554433221100"
                        "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff", key, sizeof(key)) == 32);

    n = gost_encrypt_msg(key, msg, cipher, sizeof(cipher));
    assert(n == 24);
    assert(hex_equal(cipher, (size_t)n,
                     "3B1F876E70BC17B76C0A45121EF5B36C3B683FA1310A80C1"));

    m = gost_decrypt_msg(key, cipher, (size_t)n, plain, sizeof(plain));
    assert(m == (int)strlen(msg));
    assert(strcmp(plain, msg) == 0);

    c_print_format(" <green> -> ГОСТ: УСПЕШНО\n\n");
}

static void test_kuznyechik_vector(void) {
    uint8_t key[32];
    uint8_t block[16];
    uint8_t rk[10][16];

    c_print_format(" <green> [3/5] <reset> Кузнечик: тестовый вектор...\n");

    assert(hex_to_bytes("8899aabbccddeeff0011223344556677"
                        "fedcba98765432100123456789abcdef", key, sizeof(key)) == 32);
    assert(hex_to_bytes("1122334455667700ffeeddccbbaa9988", block, sizeof(block)) == 16);

    kuznyechik_keyexp(key, rk);
    kuznyechik_encrypt(rk, block);
    assert(hex_equal(block, 16, "7f679d90bebc24305a468d42b9d4edcd"));

    kuznyechik_decrypt(rk, block);
    assert(hex_equal(block, 16, "1122334455667700ffeeddccbbaa9988"));

    c_print_format(" <green> -> Кузнечик: УСПЕШНО\n\n");
}

static void test_ctr_roundtrip(void) {
    uint8_t key[32];
    uint8_t iv[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    const char *msg = "CTR mode round-trip";
    size_t len = strlen(msg);
    uint8_t enc[64], dec[64];

    c_print_format(" <green> [4/5] <reset> Режим CTR: round-trip...\n");

    assert(hex_to_bytes("ffeeddccbbaa99887766554433221100"
                        "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff", key, sizeof(key)) == 32);

    idea_ctr_crypt(key, iv, (const uint8_t *)msg, enc, len);
    idea_ctr_crypt(key, iv, enc, dec, len);
    assert(memcmp(msg, dec, len) == 0);

    gost_ctr_crypt(key, iv, (const uint8_t *)msg, enc, len);
    gost_ctr_crypt(key, iv, enc, dec, len);
    assert(memcmp(msg, dec, len) == 0);

    c_print_format(" <green> -> CTR: УСПЕШНО\n\n");
}

static void test_gost_mac(void) {
    uint8_t key[32];
    uint32_t mac1, mac2;

    c_print_format(" <green> [5/5] <reset> ГОСТ: имитовставка...\n");

    assert(hex_to_bytes("ffeeddccbbaa99887766554433221100"
                        "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff", key, sizeof(key)) == 32);

    mac1 = gost_mac(key, (const uint8_t *)"message", 7);
    mac2 = gost_mac(key, (const uint8_t *)"message", 7);
    assert(mac1 == mac2);

    printf("  MAC = %08X\n", mac1);

    c_print_format(" <green> -> MAC: УСПЕШНО\n\n");
}

void test_lab03(void) {
    test_idea_vector();
    test_gost_vector();
    test_kuznyechik_vector();
    test_ctr_roundtrip();
    test_gost_mac();
}
