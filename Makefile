CC = g++
CFLAGS = -Wall -Wextra -Iinclude

TARGET = dist/crypto
SRC = main.cpp src/XMULT.cpp src/XEVKL.cpp helpers/file.cpp helpers/utils.cpp helpers/colorise.cpp helpers/c_utils.c helpers/randonm.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./dist/crypto
