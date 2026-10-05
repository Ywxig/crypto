CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Iinclude
CXXFLAGS = -Wall -Wextra -Iinclude

TARGET = dist/crypto
LIB = dist/libcrypto.a

# Исходные файлы алгоритмов и библиотек
ALGO_SRCS = src/CSR.c src/CSK.c src/VGN.c src/DFH.c src/DFH_xor.c src/IDEA.c src/RC2.c lib/math_x.c lib/table_log.c lib/ext_gcd.c
HELPERS_C = helpers/randonm.c
HELPERS_CPP = helpers/file.cpp helpers/utils.cpp helpers/colorise.cpp

# Объектные файлы для статической библиотеки
ALL_LIB_SRCS = $(ALGO_SRCS) $(HELPERS_C) $(HELPERS_CPP)
ALL_LIB_OBJS = $(ALL_LIB_SRCS:.c=.o)
ALL_LIB_OBJS := $(ALL_LIB_OBJS:.cpp=.o)

MAIN_SRC = main.cpp
TEST_SRC = test/test_math.c

.PHONY: all clean run lib

all: $(TARGET)

# Правило для создания статической библиотеки
$(LIB): $(ALL_LIB_SRCS)
	@mkdir -p dist
	@# Компилируем каждый исходник в объектный файл
	$(CC) $(CFLAGS) -c src/CSR.c -o src/CSR.o
	$(CC) $(CFLAGS) -c src/CSK.c -o src/CSK.o
	$(CC) $(CFLAGS) -c src/VGN.c -o src/VGN.o
	$(CC) $(CFLAGS) -c src/DFH.c -o src/DFH.o
	$(CC) $(CFLAGS) -c src/DFH_xor.c -o src/DFH_xor.o
	$(CC) $(CFLAGS) -c src/IDEA.c -o src/IDEA.o
	$(CC) $(CFLAGS) -c src/RC2.c -o src/RC2.o
	$(CC) $(CFLAGS) -c lib/math_x.c -o lib/math_x.o
	$(CC) $(CFLAGS) -c lib/table_log.c -o lib/table_log.o
	$(CC) $(CFLAGS) -c lib/ext_gcd.c -o lib/ext_gcd.o
	$(CC) $(CFLAGS) -c helpers/randonm.c -o helpers/randonm.o
	$(CXX) $(CXXFLAGS) -c helpers/file.cpp -o helpers/file.o
	$(CXX) $(CXXFLAGS) -c helpers/utils.cpp -o helpers/utils.o
	$(CXX) $(CXXFLAGS) -c helpers/colorise.cpp -o helpers/colorise.o
	@# Создаем статическую библиотеку ar
	ar rcs $(LIB) src/CSR.o src/CSK.o src/VGN.o src/DFH.o src/DFH_xor.o src/IDEA.o src/RC2.o lib/math_x.o lib/table_log.o lib/ext_gcd.o helpers/randonm.o helpers/file.o helpers/utils.o helpers/colorise.o

# Сборка финального бинарника с использованием статической библиотеки и тестов
$(TARGET): $(LIB) $(MAIN_SRC) $(TEST_SRC)
	@mkdir -p dist
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) $(TEST_SRC) $(LIB) -o $(TARGET)
	@rm -f $(ALL_LIB_OBJS)

lib: $(LIB)

clean:
	rm -rf dist
	rm -f src/*.o lib/*.o helpers/*.o

run: all
	./$(TARGET)
