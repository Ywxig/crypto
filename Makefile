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
ALGO_SRCS = src/CSR.c src/CSK.c src/VGN.c src/DFH.c src/DFH_xor.c src/IDEA.c src/RC2.c src/DFHC.c
MATH_SRCS = lib/math_x.c lib/table_log.c lib/ext_gcd.c
HELPERS_C = helpers/randonm.c
HELPERS_CPP = helpers/file.cpp helpers/utils.cpp helpers/colorise.cpp

# Объектные файлы
ALGO_OBJS = $(ALGO_SRCS:.c=.o)
MATH_OBJS = $(MATH_SRCS:.c=.o)
HELPERS_C_OBJS = $(HELPERS_C:.c=.o)
HELPERS_CPP_OBJS = $(HELPERS_CPP:.cpp=.o)

ALL_OBJS = $(ALGO_OBJS) $(MATH_OBJS) $(HELPERS_C_OBJS) $(HELPERS_CPP_OBJS)

MAIN_SRC = main.cpp
TEST_SRC = test/test_math.c

.PHONY: all clean run lib

all: $(TARGET)

# Шаблонные правила компиляции для .c и .cpp файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Правило для статической библиотеки математики (libmath.a)
$(LIB_MATH): $(MATH_OBJS)
	@mkdir -p dist
	ar rcs $(LIB_MATH) $(MATH_OBJS)

# Правило для статической библиотеки криптоалгоритмов (libcrypto.a)
$(LIB_CRYPTO): $(ALGO_OBJS) $(HELPERS_C_OBJS) $(HELPERS_CPP_OBJS) $(LIB_MATH)
	@mkdir -p dist
	ar rcs $(LIB_CRYPTO) $(ALGO_OBJS) $(HELPERS_C_OBJS) $(HELPERS_CPP_OBJS)

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
