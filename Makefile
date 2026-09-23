CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Iinclude
CXXFLAGS = -Wall -Wextra -Iinclude

TARGET = dist/crypto
SRC = main.cpp src/CSR.c src/CSK.c src/VGN.c src/DFH.c
HELPERS = helpers/file.cpp helpers/utils.cpp helpers/colorise.cpp helpers/c_utils.c helpers/randonm.c
LIBS = lib/math_x.c lib/table_log.c

# Объединяем все исходные файлы
ALL_SRCS = $(SRC) $(HELPERS) $(LIBS)

.PHONY: all clean run

all: $(TARGET)

# Создаем папку dist перед компиляцией
$(TARGET): $(ALL_SRCS)
	@mkdir -p dist
	$(CXX) $(CXXFLAGS) $(ALL_SRCS) -o $(TARGET)

clean:
	rm -rf dist

run: all
	./$(TARGET)
