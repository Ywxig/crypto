#ifndef TEST_MATH_H
#define TEST_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

int START_TEST(void);
void test_galois_field(void);
void test_modular_exponentiation(void);
void test_modular_inverse(void);
void test_discrete_logarithm(void);

#ifdef __cplusplus
}
#endif

#endif // TEST_MATH_H
