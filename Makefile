CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Iinclude
CXXFLAGS = -Wall -Wextra -Iinclude

TARGET = dist/crypto

# Две раздельные статические библиотеки:
# 1. libcrypto.a для алгоритмов шифрования из src/
# 2. libmath.a для математических функций из lib/
LIB_CRYPTO = dist/libcrypto.a
LIB_MATH = dist/libmath.a

# Исходные файлы
ALGO_SRCS = src/CSR.c src/CSK.c src/VGN.c src/DFH.c src/DFH_xor.c src/IDEA.c src/RC2.c
MATH_SRCS = lib/math_x.c lib/table_log.c lib/ext_gcd.c
HELPERS_C = helpers/randonm.c
HELPERS_CPP = helpers/file.cpp helpers/utils.cpp helpers/colorise.cpp

ALL_OBJS = $(ALGO_SRCS:.c=.o) $(MATH_SRCS:.c=.o) $(HELPERS_C:.c=.o) $(HELPERS_CPP:.cpp=.o)

MAIN_SRC = main.cpp
TEST_SRC = test/test_math.c

.PHONY: all clean run lib

all: $(TARGET)

# Правило для статической библиотеки математики (libmath.a)
$(LIB_MATH): $(MATH_SRCS)
	@mkdir -p dist
	$(CC) $(CFLAGS) -c lib/math_x.c -o lib/math_x.o
	$(CC) $(CFLAGS) -c lib/table_log.c -o lib/table_log.o
	$(CC) $(CFLAGS) -c lib/ext_gcd.c -o lib/ext_gcd.o
	ar rcs $(LIB_MATH) lib/math_x.o lib/table_log.o lib/ext_gcd.o

# Правило для статической библиотеки криптоалгоритмов (libcrypto.a)
$(LIB_CRYPTO): $(ALGO_SRCS) $(HELPERS_C) $(HELPERS_CPP) $(LIB_MATH)
	@mkdir -p dist
	$(CC) $(CFLAGS) -c src/CSR.c -o src/CSR.o
	$(CC) $(CFLAGS) -c src/CSK.c -o src/CSK.o
	$(CC) $(CFLAGS) -c src/VGN.c -o src/VGN.o
	$(CC) $(CFLAGS) -c src/DFH.c -o src/DFH.o
	$(CC) $(CFLAGS) -c src/DFH_xor.c -o src/DFH_xor.o
	$(CC) $(CFLAGS) -c src/IDEA.c -o src/IDEA.o
	$(CC) $(CFLAGS) -c src/RC2.c -o src/RC2.o
	$(CC) $(CFLAGS) -c helpers/randonm.c -o helpers/randonm.o
	$(CXX) $(CXXFLAGS) -c helpers/file.cpp -o helpers/file.o
	$(CXX) $(CXXFLAGS) -c helpers/utils.cpp -o helpers/utils.o
	$(CXX) $(CXXFLAGS) -c helpers/colorise.cpp -o helpers/colorise.o
	ar rcs $(LIB_CRYPTO) src/CSR.o src/CSK.o src/VGN.o src/DFH.o src/DFH_xor.o src/IDEA.o src/RC2.o helpers/randonm.o helpers/file.o helpers/utils.o helpers/colorise.o

lib: $(LIB_MATH) $(LIB_CRYPTO)

# Сборка финального бинарника с использованием обеих библиотек и тестов
$(TARGET): $(LIB_MATH) $(LIB_CRYPTO) $(MAIN_SRC) $(TEST_SRC)
	@mkdir -p dist
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) $(TEST_SRC) $(LIB_CRYPTO) $(LIB_MATH) -o $(TARGET)
	@rm -f $(ALL_OBJS)

clean:
	rm -rf dist
	rm -f src/*.o lib/*.o helpers/*.o

run: all
	./$(TARGET)
