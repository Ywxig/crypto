CC = g++
CFLAGS = -Wall -Wextra -Iinclude

TARGET = dist/main
SRC = main.cpp src/CSR.c src/CSK.c src/VGN.c src/DFH.c helpers/file.cpp helpers/utils.cpp helpers/colorise.cpp helpers/c_utils.c helpers/randonm.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./dist/main
