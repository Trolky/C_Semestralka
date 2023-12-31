CC = gcc
CFLAGS = -Wall -pedantic -ansi
LDFLAGS = -lpng -lz
BIN = stegim.exe
OBJ = src/file.o src/pngImage.o src/bmp.o src/dictionary.o src/lzw.o src/main.o src/steganography.o

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

