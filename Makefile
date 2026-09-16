CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = dist/main
SRC = main.c src/CSR.c src/CSK.c src/VGN.c helpers/fileReader.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./dist/main
