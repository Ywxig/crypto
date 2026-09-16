CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = main
SRC = main.c src/CSR.c src/CSK.c src/VGN.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./main
